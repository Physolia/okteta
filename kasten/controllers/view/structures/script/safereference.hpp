/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2013 Alex Richardson <alex.richardson@gmx.de>
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
#ifndef KASTEN_SAFEREFERENCE_HPP
#define KASTEN_SAFEREFERENCE_HPP

#include "../structlogging.hpp"

#include <QMultiHash>
#include <QMetaType>

class DataInformation;
class SafeReference;

class SafeReferenceHolder
{
    Q_DISABLE_COPY(SafeReferenceHolder)
    SafeReferenceHolder();
    ~SafeReferenceHolder();

public:
    /** sets all refereces to this object to null */
    void invalidateAll(DataInformation* data);
    void safeReferenceDestroyed(SafeReference* ref);
    void registerSafeReference(SafeReference* ref, DataInformation* data);
    static SafeReferenceHolder instance;

private:
    using Container = QMultiHash<DataInformation*, SafeReference*>;
    int safeRefDestroyCnt = 0;
    int safeRefRegisterCnt = 0;
    Container mRefs;
};

/** A class wrapping a DataInformation*
 * As soon as that DataInformation is destroyed all references to it are invalidated
 */
class SafeReference
{
public:
    SafeReference();
    explicit SafeReference(DataInformation* data);
    SafeReference(const SafeReference& other);
    ~SafeReference();
    SafeReference& operator=(const SafeReference& other) = delete;
    inline DataInformation* data() const;

private:
    friend class SafeReferenceHolder;
    inline void invalidate();

private:
    DataInformation* mData = nullptr;
};

Q_DECLARE_METATYPE(SafeReference)

inline SafeReference::SafeReference()
{
    qCDebug(LOG_KASTEN_OKTETA_CONTROLLERS_STRUCTURES) << "default constructed safe reference";
}

inline SafeReference::SafeReference(DataInformation* data) : mData(data)
{
    SafeReferenceHolder::instance.registerSafeReference(this, mData);
}

inline SafeReference::SafeReference(const SafeReference& other) : mData(other.mData)
{
    SafeReferenceHolder::instance.registerSafeReference(this, mData);
}

inline SafeReference::~SafeReference()
{
    SafeReferenceHolder::instance.safeReferenceDestroyed(this);
}

inline DataInformation* SafeReference::data() const
{
    return mData;
}

inline void SafeReference::invalidate()
{
    mData = nullptr;
}

inline void SafeReferenceHolder::safeReferenceDestroyed(SafeReference* ref)
{
    if (!ref->data()) {
        return; // has been invalidated -> was already removed
    }
    int removed = mRefs.remove(ref->data(), ref);
    // qCDebug(LOG_KASTEN_OKTETA_CONTROLLERS_STRUCTURES) << "safe ref destroyed " << ref;
    if (removed <= 0) {
        qCDebug(LOG_KASTEN_OKTETA_CONTROLLERS_STRUCTURES) << "safe refenrece could not be removed:" << ref;
    } else {
        safeRefDestroyCnt += removed;
    }
}

inline void SafeReferenceHolder::registerSafeReference(SafeReference* ref, DataInformation* data)
{
    if (Q_LIKELY(data)) {
        mRefs.insert(data, ref);
        // qCDebug(LOG_KASTEN_OKTETA_CONTROLLERS_STRUCTURES) << "registered safe ref" << ref << data;
        safeRefRegisterCnt++;
    } else {
        qCWarning(LOG_KASTEN_OKTETA_CONTROLLERS_STRUCTURES) << "invalid ref copied";
    }
}

#endif // KASTEN_SAFEREFERENCE_HPP
