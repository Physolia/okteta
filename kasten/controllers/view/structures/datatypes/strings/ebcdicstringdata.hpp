/*
 *   This file is part of the Okteta Kasten Framework, made within the KDE community.
 *
 *   Copyright 2011 Alex Richardson <alex.richardson@gmx.de>
 *   Copyright 2016 Aaron Bishop <erroneous@gmail.com>
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

#ifndef EBCDICSTRINGDATA_HPP
#define EBCDICSTRINGDATA_HPP

#include "stringdata.hpp"

#include <QVector>

namespace Okteta {
class CharCodec;
}

class EbcdicStringData : public StringData
{
public:
    explicit EbcdicStringData(StringDataInformation* parent);
    ~EbcdicStringData() override;

    qint64 read(Okteta::AbstractByteArrayModel* input, Okteta::Address address, BitCount64 bitsRemaining) override;
    BitCount32 sizeAt(uint i) const override;
    BitCount32 size() const override;
    QString completeString(bool skipInvalid = false) const override;
    QString stringValue(int row) const override;
    QString charType() const override;
    uint count() const override;
    QString typeName() const override;

private:
    QVector<quint8> mData;
    Okteta::CharCodec* mCodec;
};

#endif // EBCDICSTRINGDATA_HPP