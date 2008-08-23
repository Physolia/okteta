/*
    This file is part of the Kakao Framework, part of the KDE project.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "connectorcontroller.h"

// Kakao core
#include <kdocumentsyncmanager.h>
// KDE
#include <KActionCollection>
#include <KAction>
#include <KXMLGUIClient>
#include <KConfigGroup>
#include <KLocale>


ConnectorController::ConnectorController( KDocumentSyncManager* syncManager, KXMLGUIClient* guiClient )
: mSyncManager( syncManager )
{
    KActionCollection* actionCollection = guiClient->actionCollection();

    KAction* connectAction = actionCollection->addAction( "connect_to" );
    connectAction->setIcon( KIcon("network-connect") );
    connectAction->setText( i18nc("@action:inmenu","&Connect to...") );
    connect( connectAction, SIGNAL(triggered(bool) ), SLOT(connectTo()) );
    connectAction->setShortcut( Qt::CTRL + Qt::Key_T );
}


void ConnectorController::setTargetModel( AbstractModel* model )
{
Q_UNUSED( model )
}

void ConnectorController::connectTo()
{
    mSyncManager->connectTo();
}

ConnectorController::~ConnectorController()
{
}
