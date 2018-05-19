/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2009, 2010, 2011  Alex Richardson <alex.richardson@gmx.de>
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
#ifndef PRIMITIVEDATAINFORMATION_H_
#define PRIMITIVEDATAINFORMATION_H_

#include "../datainformation.h"
#include "../primitivedatatype.h"
#include "../../allprimitivetypes.h"

union AllPrimitiveTypes;

/**
 * A base class for all primitive data elements (e.g. unsigned integers or floating-point numbers)
 */
class PrimitiveDataInformation : public DataInformation
{
    friend class PrimitiveDataInformationTest; // that unit test needs to change mWasAbleToRead

public:
    explicit PrimitiveDataInformation(const QString& name, DataInformation* parent = nullptr);
    ~PrimitiveDataInformation() override;
    PrimitiveDataInformation* clone() const override = 0;

    Qt::ItemFlags flags(int column, bool fileLoaded = true) const override;

    bool isPrimitive() const override;
    virtual AllPrimitiveTypes value() const = 0;
    virtual void setValue(AllPrimitiveTypes newValue) = 0;
    virtual QScriptValue valueAsQScriptValue() const = 0;
    virtual PrimitiveDataType type() const = 0;

    unsigned int childCount() const override;
    DataInformation* childAt(unsigned int) const override;
    bool canHaveChildren() const override;
    BitCount64 childPosition(const DataInformation*, Okteta::Address) const override;
    int indexOf(const DataInformation* const) const override;

    /** @return the matching prefix for the base (nothing, '0x', '0b' or '0o') */
    static QString basePrefix(int base);

protected:
    virtual BitCount32 offset(unsigned int index) const;
    PrimitiveDataInformation(const PrimitiveDataInformation& d);
};

/**
 * A base class for data types which just wrap an underlying primitive data type.
 * The basic operations (reading/writing/size/value) are simply delegated to the wrapped element.
 *
 * This is used for pointers (wrapping differently sized integers) and enumerations/bitflags
 * Due to the use of a PrimitiveDataInformation pointer as a member any primitive type can be wrapped.
 * For example pointers using a bitfield value are possible and theoretically floating point enums
 * (although these were not added due to float comparison issues - use the binary representation of an
 * equally sized integer instead).
 */
class PrimitiveDataInformationWrapper : public PrimitiveDataInformation
{
protected:
    PrimitiveDataInformationWrapper(const PrimitiveDataInformationWrapper& d);

public:
    /** takes ownership of @p valueType */
    PrimitiveDataInformationWrapper(const QString& name, PrimitiveDataInformation* valueType,
                                    DataInformation* parent = nullptr);
    ~PrimitiveDataInformationWrapper() override {}

    // delegate all these to the underlying object:

    bool setData(const QVariant& value, Okteta::AbstractByteArrayModel* out,
                 Okteta::Address address, BitCount64 bitsRemaining, quint8 bitOffset) override;

    BitCount32 size() const override;

    void setWidgetData(QWidget* w) const override;

    QVariant dataFromWidget(const QWidget* w) const override;

    QWidget* createEditWidget(QWidget* parent) const override;

    AllPrimitiveTypes value() const override;

    void setValue(AllPrimitiveTypes newValue) override;

    PrimitiveDataType type() const override;

    QScriptValue valueAsQScriptValue() const override;

    // we have to do slightly more than just forward with this method
    qint64 readData(Okteta::AbstractByteArrayModel* input, Okteta::Address address,
                    BitCount64 bitsRemaining, quint8* bitOffset) override;

protected:
    QScopedPointer<PrimitiveDataInformation> mValue;
};

inline PrimitiveDataInformation::PrimitiveDataInformation(const QString& name,
                                                          DataInformation* parent)
    : DataInformation(name, parent)
{
}

inline PrimitiveDataInformation::PrimitiveDataInformation(const PrimitiveDataInformation& d)
    : DataInformation(d)
{
}

inline QString PrimitiveDataInformation::basePrefix(int base)
{
    switch (base) {
    case 10: return QString();
    case 2: return QStringLiteral("0b");
    case 8: return QStringLiteral("0o");
    case 16: return QStringLiteral("0x");
    default:
        Q_ASSERT_X(false, "PrimitiveDataInformation::basePrefix()", "Invalid argument!");
        return QString();
    }
}

#endif /* PRIMITIVEDATAINFORMATION_H_ */
