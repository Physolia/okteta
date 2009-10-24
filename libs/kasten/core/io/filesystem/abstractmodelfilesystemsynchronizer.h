/*
    This file is part of the Kasten Framework, part of the KDE project.

    Copyright 2007-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef ABSTRACTMODELFILESYSTEMSYNCHRONIZER_H
#define ABSTRACTMODELFILESYSTEMSYNCHRONIZER_H

// lib
#include <abstractmodelsynchronizer.h>


namespace Kasten
{
class AbstractModelFileSystemSynchronizerPrivate;


class KASTENCORE_EXPORT AbstractModelFileSystemSynchronizer : public AbstractModelSynchronizer
{
  Q_OBJECT

  friend class AbstractFileSystemLoadJobPrivate;
  friend class AbstractFileSystemConnectJobPrivate;
  friend class AbstractFileSystemSyncWithRemoteJobPrivate;
  friend class AbstractFileSystemSyncToRemoteJobPrivate;

  protected:
    enum FileChange { FileDirty, FileCreated, FileDeleted };

  protected:
    explicit AbstractModelFileSystemSynchronizer( AbstractModelFileSystemSynchronizerPrivate* d );

  public:
    AbstractModelFileSystemSynchronizer();

    virtual ~AbstractModelFileSystemSynchronizer();

  protected: // API to be implemented
    virtual void handleFileChange( FileChange change ) = 0;

  protected:
    Q_PRIVATE_SLOT( d_func(), void onFileDirty( const QString& filePath ) )
    Q_PRIVATE_SLOT( d_func(), void onFileCreated( const QString& filePath ) )
    Q_PRIVATE_SLOT( d_func(), void onFileDeleted( const QString& filePath ) )

  protected:
    Q_DECLARE_PRIVATE( AbstractModelFileSystemSynchronizer )
};

}

#endif
