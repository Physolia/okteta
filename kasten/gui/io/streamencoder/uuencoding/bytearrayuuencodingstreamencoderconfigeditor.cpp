/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2010 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "bytearrayuuencodingstreamencoderconfigeditor.h"

// lib
#include "bytearraytextstreamencoderpreview.h"
// KF5
#include <KLocalizedString>
#include <KComboBox>
// Qt
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>


namespace Kasten
{

ByteArrayUuencodingStreamEncoderConfigEditor::ByteArrayUuencodingStreamEncoderConfigEditor( ByteArrayUuencodingStreamEncoder* encoder, QWidget* parent )
 : AbstractModelStreamEncoderConfigEditor( parent ),
   mEncoder( encoder )
{
    mSettings = mEncoder->settings();

    QFormLayout* pageLayout = new QFormLayout( this );
    pageLayout->setMargin( 0 );

    // internal file name
    const QString fileNameLabel =
        i18nc( "@label:textbox file name internally given to the encoded data",
               "Internal name of file:" );

    mFileNameEdit = new QLineEdit( this );
    mFileNameEdit->setClearButtonEnabled( true );
    mFileNameEdit->setText( mSettings.fileName );
    connect( mFileNameEdit, &QLineEdit::textChanged, this, &ByteArrayUuencodingStreamEncoderConfigEditor::onSettingsChanged );
    pageLayout->addRow( fileNameLabel, mFileNameEdit );

    // data type
    const QString encodingTypeLabel =
        i18nc( "@label:listbox the type of the used encoding: historical or Base64.",
               "Encoding:" );

    mEncodingSelect = new KComboBox( this );
    const QStringList list {
        i18nc("@item:inmenu Doing the uuencoding using the historical encoding",
              "Historical"),
        i18nc("@item:inmenu Doing the uuencoding using the base64 encoding",
              "Base64"),
    };
    mEncodingSelect->addItems( list );
    mEncodingSelect->setCurrentIndex( static_cast<int>(mSettings.algorithmId) );
    connect( mEncodingSelect, QOverload<int>::of(&KComboBox::activated),
             this, &ByteArrayUuencodingStreamEncoderConfigEditor::onSettingsChanged );
    pageLayout->addRow( encodingTypeLabel, mEncodingSelect );
}

AbstractSelectionView* ByteArrayUuencodingStreamEncoderConfigEditor::createPreviewView() const
{
    return new ByteArrayTextStreamEncoderPreview( mEncoder );
}

void ByteArrayUuencodingStreamEncoderConfigEditor::onSettingsChanged()
{
    mSettings.algorithmId = static_cast<UuencodingStreamEncoderSettings::AlgorithmId>(mEncodingSelect->currentIndex());
    mSettings.fileName = mFileNameEdit->text();

    mEncoder->setSettings( mSettings );
}

ByteArrayUuencodingStreamEncoderConfigEditor::~ByteArrayUuencodingStreamEncoderConfigEditor()
{
}

}
