/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef UINT32_H
#define UINT32_H

// Qt
#include <QMetaType>
#include <QString>

struct UInt32
{
public:
    UInt32(quint32 v);
    UInt32();

public:
    QString toString(bool asHex) const;

public:
    quint32 value;
};

inline UInt32::UInt32() : value(0) {}
inline UInt32::UInt32(quint32 v) : value(v) {}

inline QString UInt32::toString(bool asHex) const
{
    return asHex ? QStringLiteral("0x%1").arg(value, 8, 16, QChar::fromLatin1('0')) :
                   QString::number(value);
}

Q_DECLARE_METATYPE(UInt32)

#endif
