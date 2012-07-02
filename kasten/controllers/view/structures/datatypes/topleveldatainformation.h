/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2010, 2011 Alex Richardson <alex.richardson@gmx.de>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) version 3, or any
 *   later version accepted by the membership of KDE e.V. (or its
 *   successor approved by the membership of KDE e.V.), which shall
 *   act as a proxy defined in Section 6 of version 3 of the license.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TOPLEVELDATAINFORMATION_H_
#define TOPLEVELDATAINFORMATION_H_

#include <QtCore/QMap>
#include <QtCore/QFileInfo>
#include <QtCore/QExplicitlySharedDataPointer>

#include <arraychangemetricslist.h>
#include "datainformationbase.h"
#include "../script/scripthandler.h"

class ScriptLogger;
class ScriptHandlerInfo;
namespace Okteta
{
class AbstractByteArrayModel;
} // namespace Okteta

class QScriptEngine;
class ScriptHandler;
class DataInformation;

class TopLevelDataInformation : public QObject, public DataInformationBase
{
Q_OBJECT
    Q_DISABLE_COPY(TopLevelDataInformation)
public:
    /** create a new TopLevelDataInformation wrapping @p data
     *  @param data the object to wrap (takes ownership). If data is dummy then this object is invalid
     *  @param logger the logger to use (takes ownership). If null a new one is created
     *  @param engine the script engine to use. If null this object contains no dynamic elements
     *  @param structureFile the file which this was loaded from
     */
    explicit TopLevelDataInformation(DataInformation* data, ScriptLogger* logger = 0,
            QScriptEngine* engine = 0, const QFileInfo& structureFile = QFileInfo());
    virtual ~TopLevelDataInformation();

    typedef QSharedPointer<TopLevelDataInformation> Ptr;
    typedef QVector<Ptr> List;

public:
    void validate();
    /** Reads the necessary data from @p input
     *
     * @param input the byte array to read from
     * @param byteOrder the byte order used to read the data (big/little endian), may be overridden by child structures
     * @param address the starting offset to read from, or if an offset is locked, the locked offset
     * @param changesList the list with changes to @input, so that it is possible to check whether reading is necessary
     *      This parameter is only useful if the structure was locked to a specific position.
     * @param forceRead whether to always read data, ignoring the change list
     */
    void read(Okteta::AbstractByteArrayModel* input, Okteta::Address address,
            const Okteta::ArrayChangeMetricsList& changesList, bool forceRead);
    void updateElement(DataInformation* elem);
    QScriptEngine* scriptEngine() const;

    DataInformation* actualDataInformation() const;
    bool isValid() const;
    void lockPositionToOffset(Okteta::Address offset, const Okteta::AbstractByteArrayModel* model);
    void unlockPosition(const Okteta::AbstractByteArrayModel* model);
    bool isLockedFor(const Okteta::AbstractByteArrayModel* model) const;
    quint64 lockPositionFor(const Okteta::AbstractByteArrayModel* model) const;
    int indexOf(const DataInformation* const data) const;
    int index() const;
    void setIndex(int newIndex);
    ScriptHandler* scriptHandler() const;
    ScriptLogger* logger() const;
    void setChildDataChanged();

    virtual bool isTopLevel() const;

    //only public so that DataInformation can call them
    void _childrenAboutToBeInserted(DataInformation* sender, uint startIndex, uint endIndex);
    void _childrenInserted(const DataInformation* sender, uint startIndex, uint endIndex);
    void _childrenAboutToBeRemoved(DataInformation* sender, uint startIndex, uint endIndex);
    void _childrenRemoved(const DataInformation* sender, uint startIndex, uint endIndex);

private:
    bool isReadingNecessary(const Okteta::ArrayChangeMetricsList& changesList, Okteta::Address address);

public Q_SLOTS:
    void resetValidationState();
    void removeByteArrayModelFromList(QObject* model);

Q_SIGNALS:
    void dataChanged();
    /** items are inserted before @p startIndex */
    void childrenAboutToBeInserted(DataInformation* sender, uint startIndex, uint endIndex);
    /** items are inserted before @p startIndex */
    void childrenInserted(const DataInformation* sender, uint startIndex, uint endIndex);
    /** items are removed before @p startIndex */
    void childrenAboutToBeRemoved(DataInformation* sender, uint startIndex, uint endIndex);
    /** items are inserted before @p startIndex */
    void childrenRemoved(const DataInformation* sender, uint startIndex, uint endIndex);

private:
    QScopedPointer<DataInformation> mData;
    QScopedPointer<ScriptHandler> mScriptHandler;
    QScopedPointer<ScriptLogger> mLogger;
    QFileInfo mStructureFile;
    /** Save the position this structure is locked to for each ByteArrayModel
     * QObject::destroyed() has to be connected to slot removeByteArrayModel()
     *  so that no dangling pointers remain
     *  Keys are the models and values are a pointer so that NULL can indicate that it is not locked.
     */
    QMap<const Okteta::AbstractByteArrayModel*, quint64> mLockedPositions;
    int mIndex;
    bool mValid :1;
    bool mChildDataChanged :1;
};

inline DataInformation* TopLevelDataInformation::actualDataInformation() const
{
    return mData.data();
}

inline bool TopLevelDataInformation::isValid() const
{
    return mValid;
}

inline int TopLevelDataInformation::index() const
{
    return mIndex;
}

inline void TopLevelDataInformation::setIndex(int newIndex)
{
    mIndex = newIndex;
}

inline void TopLevelDataInformation::setChildDataChanged()
{
    mChildDataChanged = true;
}

inline void TopLevelDataInformation::_childrenAboutToBeInserted(DataInformation* sender,
        uint startIndex, uint endIndex)
{
    emit childrenAboutToBeInserted(sender, startIndex, endIndex);
}

inline void TopLevelDataInformation::_childrenAboutToBeRemoved(DataInformation* sender,
        uint startIndex, uint endIndex)
{
    emit childrenAboutToBeRemoved(sender, startIndex, endIndex);
}

inline void TopLevelDataInformation::_childrenInserted(const DataInformation* sender,
        uint startIndex, uint endIndex)
{
    emit childrenInserted(sender, startIndex, endIndex);
}

inline void TopLevelDataInformation::_childrenRemoved(const DataInformation* sender,
        uint startIndex, uint endIndex)
{
    emit childrenRemoved(sender, startIndex, endIndex);
}

inline bool TopLevelDataInformation::isTopLevel() const
{
    return true;
}

inline ScriptLogger* TopLevelDataInformation::logger() const
{
    return mLogger.data();
}

inline ScriptHandler* TopLevelDataInformation::scriptHandler() const
{
    return mScriptHandler.data();
}

#endif /* TOPLEVELDATAINFORMATION_H_ */
