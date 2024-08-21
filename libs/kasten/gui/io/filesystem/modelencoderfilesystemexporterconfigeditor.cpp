/*
    This file is part of the Kasten Framework, made within the KDE community.

    SPDX-FileCopyrightText: 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "modelencoderfilesystemexporterconfigeditor.hpp"

// lib
#include <abstractmodelstreamencoderconfigeditor.hpp>
// Kasten
#include <Kasten/AbstractSelectionView>
// Qt
#include <QHBoxLayout>

namespace Kasten {

ModelEncoderFileSystemExporterConfigEditor::ModelEncoderFileSystemExporterConfigEditor(AbstractModelStreamEncoderConfigEditor* encoderConfigEditor)
    : mEncoderConfigEditor(encoderConfigEditor)
{
    auto* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->addWidget(mEncoderConfigEditor.get());
}

ModelEncoderFileSystemExporterConfigEditor::~ModelEncoderFileSystemExporterConfigEditor() = default;

bool ModelEncoderFileSystemExporterConfigEditor::isValid() const
{
    return mEncoderConfigEditor->isValid();
}

std::unique_ptr<AbstractSelectionView> ModelEncoderFileSystemExporterConfigEditor::createPreviewView() const
{
    return mEncoderConfigEditor->createPreviewView();
}

}

#include "moc_modelencoderfilesystemexporterconfigeditor.cpp"
