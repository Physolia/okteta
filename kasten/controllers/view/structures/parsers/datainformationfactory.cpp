/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2012 Alex Richardson <alex.richardson@gmx.de>
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
#include "datainformationfactory.h"

#include "../datatypes/primitive/bitfield/boolbitfielddatainformation.h"
#include "../datatypes/primitive/bitfield/unsignedbitfielddatainformation.h"
#include "../datatypes/primitive/bitfield/signedbitfielddatainformation.h"
#include "../datatypes/primitivefactory.h"

#include "../script/scriptlogger.h"
#include <QScriptEngine>

AbstractBitfieldDataInformation* DataInformationFactory::newBitfield(const BitfieldParsedData& pd)
{
    if (!pd.width.isValid)
    {
        if (pd.width.string.isEmpty())
            pd.error() << "Bitfield is missing width.";
        else
            pd.error() << "Width of bitfield is not a valid number: " << pd.width.string;
        return 0;
    }
    if (pd.width.value <= 0 || pd.width.value > 64)
    {
        pd.error() << "Width of bitfield is not a value from 1-64:" << pd.width.value;
        return 0;
    }
    AbstractBitfieldDataInformation* bitf = 0;
    const QString type = pd.type.toLower();
    if (type.isEmpty())
    {
        pd.info() << "No bitfield type specified, defaulting to unsigned.";
        bitf = new UnsignedBitfieldDataInformation(pd.name, pd.width.value, pd.parent);
    }
    else if (type == QLatin1String("bool"))
        bitf = new BoolBitfieldDataInformation(pd.name, pd.width.value, pd.parent);
    else if (type == QLatin1String("unsigned"))
        bitf = new UnsignedBitfieldDataInformation(pd.name, pd.width.value, pd.parent);
    else if (type == QLatin1String("signed"))
        bitf = new SignedBitfieldDataInformation(pd.name, pd.width.value, pd.parent);
    else
    {
        pd.error() << "invalid bitfield type attribute given:" << type;
        return 0;
    }
    return bitf;
}

PrimitiveDataInformation* DataInformationFactory::newPrimitive(const PrimitiveParsedData& pd)
{
    if (pd.type.isEmpty())
    {
        pd.error() << "Type of primitive not specified, cannot create it!";
        return 0;
    }
    PrimitiveDataType primitiveType = PrimitiveFactory::typeStringToType(pd.type);
    if (primitiveType == Type_Invalid || primitiveType == Type_Bitfield)
    {
        pd.error() << "Unrecognized primitive type: " << pd.type;
        return 0;
    }
    return PrimitiveFactory::newInstance(pd.name, primitiveType, pd.logger, pd.parent);
}

namespace
{
template<class T>
T* newEnumOrFlags(const EnumParsedData& pd)
{
    const PrimitiveDataType primitiveType = PrimitiveFactory::typeStringToType(pd.type);
    if (primitiveType == Type_Invalid || primitiveType == Type_Bitfield)
    {
        pd.error() << "Unrecognized enum type: " << pd.type;
        return 0;
    }
    if (primitiveType == Type_Float || primitiveType == Type_Double)
    {
        pd.error() << "Floating-point enums are not allowed since they make little sense.";
        return 0;
    }
    EnumDefinition::Ptr definition = pd.enumDef;
    if (!definition)
    {
        QMap<AllPrimitiveTypes, QString> enumValues =
                AbstractEnumDataInformation::parseEnumValues(pd.enumValuesObject, pd.logger, primitiveType,
                        pd.context());
        if (enumValues.isEmpty())
        {
            pd.error() << "No enum values specified!";
            return 0;
        }
        definition = EnumDefinition::Ptr(new EnumDefinition(enumValues, pd.enumName, primitiveType));
    }
    if (definition->type() != primitiveType)
    {
        pd.error().nospace() << "Enum type (" << definition->type() << ") and value type (" << primitiveType
                << ") do not match!";
        return 0;
    }
    PrimitiveDataInformation* primData = PrimitiveFactory::newInstance(pd.name, primitiveType, pd.logger);
    //TODO allow bitfields?
    if (!primData)
    {
        pd.error() << "Could not create a value object for this enum!";
        return 0;
    }
    return new T(pd.name, primData, definition, pd.parent);
}

inline QScriptValue functionFromString(const QString& str, const CommonParsedData& info)
{
    Q_ASSERT(!str.isEmpty());
    //must wrap in parentheses, see https://bugreports.qt-project.org/browse/QTBUG-5757
    QScriptValue ret = info.engine->evaluate(QLatin1Char('(') + str + QLatin1Char(')'));
    if (ret.isFunction())
        return ret;
    else
        return QScriptValue();
}

}

EnumDataInformation* DataInformationFactory::newEnum(const EnumParsedData& pd)
{
    return newEnumOrFlags<EnumDataInformation>(pd);
}

FlagDataInformation* DataInformationFactory::newFlags(const EnumParsedData& pd)
{
    return newEnumOrFlags<FlagDataInformation>(pd);
}

ArrayDataInformation* DataInformationFactory::newArray(const ArrayParsedData& pd)
{
    if (!pd.arrayType)
    {
        pd.error() << "Failed to parse array type!";
        return 0;
    }
    if (pd.length < 0)
    {
        pd.error() << "Cannot create array with negative length:" << pd.length;
        return 0;
    }
    if (pd.lengthFunction.isValid())
    {
        if (!pd.lengthFunction.isFunction())
        {
            pd.error() << "Length function is not a function:" << pd.lengthFunction.toString();
            return 0;
        }
    }
    return new ArrayDataInformation(pd.name, pd.length, pd.arrayType, pd.parent, pd.lengthFunction);
}

bool DataInformationFactory::commonInitialization(DataInformation* data, const CommonParsedData& pd)
{
    data->setByteOrder(pd.endianess);
    QScriptValue update = pd.updateFunc;
    if (!pd.updateFuncString.isEmpty())
    {
        update = functionFromString(pd.updateFuncString, pd);
        if (!update.isFunction())
        {
            pd.error() << "Could not convert updateFunc to function: " << pd.updateFuncString;
            return false;
        }
    }
    QScriptValue validation = pd.validationFunc;
    if (!pd.validationFuncString.isEmpty())
    {
        validation = functionFromString(pd.validationFuncString, pd);
        if (!validation.isFunction())
        {
            pd.error() << "Could not convert validationFunc to function: " << pd.validationFuncString;
            return false;
        }
    }

    if (update.isValid())
    {
        if (!update.isFunction())
        {
            pd.error() << "Update function is not a function: " << update.toString();
            return false;
        }
        data->setUpdateFunc(update);
    }
    if (validation.isValid())
    {
        if (!validation.isFunction())
        {
            pd.error() << "Validation function is not a function: " << validation.toString();
            return false;
        }
        data->setValidationFunc(validation);
    }
    return true;

}
