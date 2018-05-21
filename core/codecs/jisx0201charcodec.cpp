/*
    This file is part of the Okteta Core library, made within the KDE community.

    Copyright 2015 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "jisx0201charcodec.h"

// lib
#include <character.h>
// Qt
#include <QString>

namespace Okteta {

bool JISX0201CharCodec::encode(Byte* byte, const QChar& _char) const
{
    const ushort charUnicode = _char.unicode();

    const unsigned char _byte =
        (0x0020 <= charUnicode && charUnicode <= 0x005B) ? charUnicode :
        (0x005D <= charUnicode && charUnicode <= 0x007D) ? charUnicode :
        (charUnicode == 0x00A5) ?                          92 :
        (charUnicode == 0x203E) ?                          126 :
        (0xFF61 <= charUnicode && charUnicode <= 0xFF9F) ? (charUnicode - 0xFF61 + 161) :
        /* else */                                         0;

    // not covered?
    if (_byte == 0) {
        return false;
    }

    *byte = _byte;

    return true;
}

Character JISX0201CharCodec::decode(Byte byte) const
{
    const ushort unicode =
        (32 <= byte && byte <= 91) ||
        (93 <= byte && byte <= 125)
            ?  ushort(byte) :
        (92 == byte)
            ?  ushort(0x00A5) :
        (126 == byte)
            ?  ushort(0x203E) :
        (161 <= byte && byte <= 223)
            ? ushort(0xFF61 - 161 + byte) :
        /* else */ ushort(0);

    return {QChar(unicode), (unicode == 0)};
}

bool JISX0201CharCodec::canEncode(const QChar& _char) const
{
    const ushort charUnicode = _char.unicode();
    return
        (0x0020 <= charUnicode && charUnicode <= 0x005B) ||
        (0x005D <= charUnicode && charUnicode <= 0x007D) ||
        (charUnicode == 0x00A5) || (charUnicode == 0x203E) ||
        (0xFF61 <= charUnicode && charUnicode <= 0xFF9F);
}

const QString& JISX0201CharCodec::name() const
{
    return codecName();
}

const QString& JISX0201CharCodec::codecName()
{
    static const QString name = QStringLiteral("JIS X 0201");
    return name;
}

}
