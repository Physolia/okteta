/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef MODELCODECVIEWMANAGER_HPP
#define MODELCODECVIEWMANAGER_HPP

// lib
#include <kasten/kastengui_export.hpp>
// Qt
#include <QObject>
#include <QVector>

class QString;

namespace Kasten {

class AbstractModelStreamEncoderConfigEditorFactory;
class AbstractModelStreamEncoderConfigEditor;
class AbstractModelStreamEncoder;
class AbstractModelExporterConfigEditorFactory;
class AbstractModelExporterConfigEditor;
class AbstractModelExporter;
class AbstractModelDataGeneratorConfigEditorFactory;
class AbstractModelDataGeneratorConfigEditor;
class AbstractModelDataGenerator;
class ViewManager;

class KASTENGUI_EXPORT ModelCodecViewManager : public QObject
{
    Q_OBJECT

public:
    ModelCodecViewManager();
    ~ModelCodecViewManager() override;

public:
    AbstractModelStreamEncoderConfigEditor* createConfigEditor(AbstractModelStreamEncoder* encoder) const;
    AbstractModelExporterConfigEditor* createConfigEditor(AbstractModelExporter* exporter) const;
    AbstractModelDataGeneratorConfigEditor* createConfigEditor(AbstractModelDataGenerator* generator) const;

public:
    void setEncoderConfigEditorFactories(const QVector<AbstractModelStreamEncoderConfigEditorFactory*>& factoryList);
    void setExporterConfigEditorFactories(const QVector<AbstractModelExporterConfigEditorFactory*>& factoryList);
    void setGeneratorConfigEditorFactories(const QVector<AbstractModelDataGeneratorConfigEditorFactory*>& factoryList);

private:
    QVector<AbstractModelStreamEncoderConfigEditorFactory*> mEncoderFactoryList;
    QVector<AbstractModelExporterConfigEditorFactory*> mExporterFactoryList;
    QVector<AbstractModelDataGeneratorConfigEditorFactory*> mGeneratorFactoryList;
};

}

#endif