/*
    This file is part of the Okteta Core library, made within the KDE community.

    SPDX-FileCopyrightText: 2004, 2011 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef OKTETA_TEXTCHARCODEC_HPP
#define OKTETA_TEXTCHARCODEC_HPP

// lib
#include <charcodec.hpp>
// Qt
#include <QString>

class QTextCodec;
class QTextDecoder;
class QTextEncoder;

namespace Okteta {

// used by all codecs with full char coping, i.e. there are no undefined chars
class TextCharCodec : public CharCodec
{
public:
    static TextCharCodec* createCodec(const QString& codecName);
    static TextCharCodec* createCodec(CharCoding charCoding);
    static TextCharCodec* createLocalCodec();

    static const QStringList& codecNames();

public:
    TextCharCodec(const TextCharCodec&) = delete;
    ~TextCharCodec() override;

    TextCharCodec& operator=(const TextCharCodec&) = delete;

protected:
    explicit TextCharCodec(QTextCodec* textCodec);

public: // CharCodec API
    Character decode(Byte byte) const override;
    bool encode(Byte* byte, QChar _char) const override;
    bool canEncode(QChar _char) const override;
    QString name() const override;

private:
    QTextCodec* mCodec;
    /** decodes the chars to unicode */
    QTextDecoder* mDecoder;
    /** encodes the chars from unicode */
    QTextEncoder* mEncoder;
    /** */
    mutable QString mName;
};

}

#endif
