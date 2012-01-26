/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2011, 2012 Alex Richardson <alex.richardson@gmx.de>
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
#include "abstractenumdatainformation.h"

#include <QtScript/QScriptValueIterator>

#include <limits>

AbstractEnumDataInformation::AbstractEnumDataInformation(QString name, EnumDefinition::Ptr enumDef, DataInformation* parent)
    : PrimitiveDataInformation(name, parent), mEnum(enumDef)
{

}

AbstractEnumDataInformation::AbstractEnumDataInformation(const AbstractEnumDataInformation& e)
    : PrimitiveDataInformation(e), mEnum(e.mEnum)
{
}

AbstractEnumDataInformation::~AbstractEnumDataInformation()
{
}

//TODO check for range of unterlying type
QMap< AllPrimitiveTypes, QString > AbstractEnumDataInformation::parseEnumValues(const QScriptValue& val, PrimitiveDataType type)
{
    QMap<AllPrimitiveTypes, QString> enumValues;
    if (type == Type_NotPrimitive || type == Type_Double || type == Type_Float)
    {
        kWarning() << "Invalid type" << type << "for an enumeration value";
        return enumValues;
    }
    QScriptValueIterator it(val);
    while (it.hasNext())
    {
        it.next();
        QScriptValue val = it.value();
        if (val.isNumber())
        {
            qsreal num = val.toNumber();
            num = qAbs(num);
            if (num <= std::numeric_limits<quint32>::max())
                enumValues.insert(val.toUInt32(), it.name());
            else
                enumValues.insert(quint64(num), it.name());
        }
        else {
            QString numStr = val.toString();
            bool ok = false;
            quint64 num;
            if (numStr.startsWith(QLatin1String("0x")))
            {
                num = numStr.mid(2).toULongLong(&ok, 16);
            }
            else
            {
                num = numStr.toULongLong(&ok, 10);
            }
            if (ok)
                enumValues.insert(num, it.name());
            else
                kDebug() << "could not convert" << numStr << "to an enum constant, name was:" << it.name();
        }
    }
    return enumValues;
}