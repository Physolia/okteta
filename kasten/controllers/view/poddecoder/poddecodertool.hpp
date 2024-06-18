/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    SPDX-FileCopyrightText: 2007-2010 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KASTEN_PODDECODERTOOL_HPP
#define KASTEN_PODDECODERTOOL_HPP

// tool
#include "poddata.hpp"
// Kasten core
#include <Kasten/AbstractTool>
// Okteta core
#include <Okteta/OktetaCore>
#include <Okteta/Address>
// Qt
#include <QVector>
// Std
#include <memory>

namespace Okteta {
class AbstractTypeCodec;
class CharCodec;
class AbstractByteArrayModel;
}

class QByteArray;

namespace Kasten {
class AbstractDifferentSizeDialog;
class ByteArrayView;

class PODDecoderTool : public AbstractTool
{
    Q_OBJECT

public:
    static inline constexpr int MaxPODSize = sizeof(double);
    // ensure strict alignment for double as needed on some architectures (e.g. PA-RISC)
    using Aligned64Bit = union { unsigned char Data[MaxPODSize]; double Dummy; };

private:
    static inline constexpr char ConfigGroupId[] = "PODDecoderTool";
    static inline constexpr char ByteOrderConfigKey[] = "ByteOrder";
    static inline constexpr char UnsignedAsHexConfigKey[] = "UnsignedAsHexadecimal";

    static inline constexpr QSysInfo::Endian DefaultByteOrder = QSysInfo::ByteOrder;
    static inline constexpr bool DefaultUnsignedAsHex = true;

public:
    PODDecoderTool();

    ~PODDecoderTool() override;

public: // AbstractTool API
//     virtual AbstractModel* targetModel() const;
    QString title() const override;

    void setTargetModel(AbstractModel* model) override;

public:
    bool isApplyable() const; // candidate for AbstractTool API
    bool isReadOnly() const;
    QVariant value(int podId) const;
    QByteArray bytes(int podId) const;
    QString nameOfPOD(int podId) const;
    int podCount() const;

    // TODO: add option to display & edit data in locale
    bool isUnsignedAsHex() const;
    QSysInfo::Endian byteOrder() const;
    const Okteta::CharCodec* charCodec() const;

public:
    void setData(const QVariant& data, int podId);
    void selectBytesInView(int podId);
    void markPOD(int podId);
    void unmarkPOD();

    void setDifferentSizeDialog(AbstractDifferentSizeDialog* differentSizeDialog);

public Q_SLOTS:
    void setUnsignedAsHex(bool unsignedAsHex);
    void setByteOrder(int byteOrder);

Q_SIGNALS: // changes to the setting currently not signalled, because only controlled by view
    void isApplyableChanged(bool isApplyable);    // candidate for AbstractTool API
    void readOnlyChanged(bool isReadOnly);
    void dataChanged();

private:
    void updateData();
    void setupDecoder();

private Q_SLOTS:
    void onCursorPositionChange(Okteta::Address pos);
    void onContentsChange();
    void onReadOnlyChanged();

    void onCharCodecChange(const QString& codecName);
//     void onUndefinedCharChanged(QChar undefinedChar);

private: // source
    ByteArrayView* mByteArrayView = nullptr;
    Okteta::AbstractByteArrayModel* mByteArrayModel = nullptr;
    Okteta::Address mCursorIndex = 0;

    bool mReadOnly : 1;
    bool mIsPodMarked : 1;

    QVector<Okteta::AbstractTypeCodec*> mTypeCodecs;
    std::unique_ptr<const Okteta::CharCodec> mCharCodec;
    AbstractDifferentSizeDialog* mDifferentSizeDialog = nullptr;

private: // settings
    bool mUnsignedAsHex : 1;

private: // decoded data
    Okteta::PODData mPODData;
    QVector<QVariant> mDecodedValueList;
    QVector<int> mDecodedValueByteCountList;
};

inline bool PODDecoderTool::isUnsignedAsHex() const { return mUnsignedAsHex; }
inline QSysInfo::Endian PODDecoderTool::byteOrder() const { return mPODData.byteOrder(); }
inline const Okteta::CharCodec* PODDecoderTool::charCodec() const { return mCharCodec.get(); }

}

#endif
