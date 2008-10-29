/*
    This file is part of the Kakao Framework, part of the KDE project.

    Copyright 2007-2008 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "copyascontroller.h"

// Kakao gui
#include <kidataselectable.h>
// Kakao core
#include <modelstreamencodethread.h>
#include <modelcodecmanager.h>
#include <kdocumentmanager.h>
#include <abstractmodelstreamencoder.h>
#include <abstractmodel.h>
// KDE
#include <KXMLGUIClient>
#include <KXMLGUIFactory>
#include <KActionCollection>
#include <KLocale>
// Qt
#include <QtCore/QBuffer>
#include <QtCore/QMimeData>
#include <QtGui/QActionGroup>
#include <QtGui/QClipboard>
#include <QtGui/QApplication>


static const char CopyAsActionListId[] = "copy_as_list";

CopyAsController::CopyAsController( KDocumentManager* documentManager, KXMLGUIClient* guiClient )
 : mDocumentManager( documentManager ), mGuiClient( guiClient ), mModel( 0 )
{
    mCopyAsActionGroup = new QActionGroup( this ); // TODO: do we use this only for the signal mapping?
    connect( mCopyAsActionGroup, SIGNAL(triggered( QAction* )), SLOT(onActionTriggered( QAction* )) );

    setTargetModel( 0 );
}

void CopyAsController::setTargetModel( AbstractModel* model )
{
    if( mModel ) mModel->disconnect( mCopyAsActionGroup );

    mModel = model ? model->findBaseModelWithInterface<KDE::If::DataSelectable*>() : 0;
    mSelectionControl = mModel ? qobject_cast<KDE::If::DataSelectable*>( mModel ) : 0;

    if( mSelectionControl )
    {
        connect( mModel, SIGNAL(hasSelectedDataChanged( bool )),
                 mCopyAsActionGroup, SLOT(setEnabled( bool )) );
    }

    updateActions();

    const bool hasSelectedData = ( mSelectionControl != 0 ) ? mSelectionControl->hasSelectedData() : false;
    mCopyAsActionGroup->setEnabled( hasSelectedData );
}

Q_DECLARE_METATYPE(AbstractModelStreamEncoder*)

void CopyAsController::updateActions()
{
    mGuiClient->unplugActionList( CopyAsActionListId );

    qDeleteAll( mCopyAsActionGroup->actions() );

    const AbstractModelSelection* selection = ( mSelectionControl != 0 ) ? mSelectionControl->modelSelection() : 0;

    const QList<AbstractModelStreamEncoder*> encoderList =
        mDocumentManager->codecManager()->encoderList( mModel, selection );
    const bool hasEncoders = ( encoderList.size() > 0 );

    if( hasEncoders )
    {
        for( int c = 0; c < encoderList.size(); ++c )
        {
            AbstractModelStreamEncoder *encoder = encoderList.at( c );
            const QString title = encoder->remoteTypeName();
            QAction *action = new QAction( title, mCopyAsActionGroup );

            action->setData( QVariant::fromValue(encoder) );
            mCopyAsActionGroup->addAction( action );
        }
    }
    else
    {
        QAction *noneAction = new QAction( i18nc("@item There are no encoders.","Not available."), mCopyAsActionGroup );
        mCopyAsActionGroup->addAction( noneAction );
    }
    mCopyAsActionGroup->setEnabled( hasEncoders );

    mGuiClient->plugActionList( CopyAsActionListId, mCopyAsActionGroup->actions() );
}

void CopyAsController::onActionTriggered( QAction *action )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    AbstractModelStreamEncoder *encoder = action->data().value<AbstractModelStreamEncoder *>();

    const AbstractModelSelection* selection = mSelectionControl->modelSelection();

    QByteArray exportData;
    QBuffer exportDataBuffer( &exportData );
    exportDataBuffer.open( QIODevice::WriteOnly );

    ModelStreamEncodeThread *encodeThread =
        new ModelStreamEncodeThread( this, &exportDataBuffer, mModel, selection, encoder );
    encodeThread->start();
    while( !encodeThread->wait(100) )
        QApplication::processEvents( QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers, 100 );

    delete encodeThread;

    exportDataBuffer.close();

    QMimeData *mimeData = new QMimeData;
    mimeData->setData( encoder->remoteMimeType(), exportData );
    QApplication::clipboard()->setMimeData( mimeData, QClipboard::Clipboard );

    QApplication::restoreOverrideCursor();
}
