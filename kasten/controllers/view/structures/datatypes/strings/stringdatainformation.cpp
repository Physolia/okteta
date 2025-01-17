/*
    This file is part of the Okteta Kasten Framework, made within the KDE community.

    SPDX-FileCopyrightText: 2011, 2012 Alex Richardson <alex.richardson@gmx.de>
    SPDX-FileCopyrightText: 2016 Aaron Bishop <erroneous@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "stringdatainformation.hpp"
#include "../dummydatainformation.hpp"
#include "../topleveldatainformation.hpp"
#include "utf32stringdata.hpp"
#include "utf16stringdata.hpp"
#include "utf8stringdata.hpp"
#include "asciistringdata.hpp"
#include "ebcdicstringdata.hpp"
#include "latin1stringdata.hpp"
#include "../../script/classes/stringscriptclass.hpp"
#include "../../script/scripthandlerinfo.hpp"
#include "../../script/scriptlogger.hpp"

#include <QScriptEngine>
#include <QBrush>
#include <QString>

#include <KLocalizedString>

const QString StringDataInformation::encodingNames[static_cast<int>(StringDataInformation::StringType::EBCDIC)+2] = {
    QStringLiteral("<invalid>"), // -1
    QStringLiteral("ascii"),
    QStringLiteral("latin1"),
    QStringLiteral("utf-8"),
    QStringLiteral("utf-16le"),
    QStringLiteral("utf-16-be"),
    QStringLiteral("utf32-le"),
    QStringLiteral("utf32-be"),
    QStringLiteral("ebcdic"),
};

StringDataInformation::StringDataInformation(const QString& name, StringType encoding, DataInformationBase* parent)
    : DataInformationWithDummyChildren(name, parent)
    , mDummy(this)
    , mData(nullptr)
{
    mDummy.setWasAbleToRead(true);
    setEncoding(encoding); // sets mData
}

StringDataInformation::StringDataInformation(const StringDataInformation& d)
    : DataInformationWithDummyChildren(d)
    , mDummy(this)
{
    mDummy.setWasAbleToRead(true);
    setEncoding(d.mEncoding); // sets mData
    mData->copyTerminationFrom(d.mData.get());
}

StringDataInformation::~StringDataInformation() = default;

DataInformation* StringDataInformation::childAt(unsigned int index) const
{
    Q_ASSERT(index < childCount());
    mDummy.setDummyIndex(index);
    return const_cast<DummyDataInformation*>(&mDummy);
}

bool StringDataInformation::setData(const QVariant&, Okteta::AbstractByteArrayModel*,
                                    Okteta::Address, BitCount64, quint8)
{
    Q_ASSERT_X(false, "StringDataInformation::setData()", "this should never be called!");
    return false;
}

bool StringDataInformation::setChildData(uint row, const QVariant& value, Okteta::AbstractByteArrayModel* out, Okteta::Address address, BitCount64 bitsRemaining, quint8 bitOffset)
{
    Q_UNUSED(row)
    Q_UNUSED(value)
    Q_UNUSED(out)
    Q_UNUSED(address)
    Q_UNUSED(bitsRemaining)
    Q_UNUSED(bitOffset)
    logWarn() << "setChildData not implemented yet!";
    return false;
}

qint64 StringDataInformation::readData(const Okteta::AbstractByteArrayModel* input, Okteta::Address address,
                                       BitCount64 bitsRemaining, quint8* bitOffset)
{
    Q_ASSERT(mHasBeenUpdated); // update must have been called prior to reading
    if (*bitOffset != 0) {
        logWarn() << "while reading string bit offset was: " << *bitOffset
                  << ", adding padding and continuing at next byte (address=" << address << ")";
        bitsRemaining -= 8 - *bitOffset;
        *bitOffset = 0;
        address += 1;
        Q_ASSERT((bitsRemaining % 8) == 0); // must be mod 8
    }
    qint64 ret = mData->read(input, address, bitsRemaining);
    mWasAbleToRead = ret >= 0;
    return ret;
}

BitCount32 StringDataInformation::size() const
{
    return mData->size();
}

void StringDataInformation::setWidgetData(QWidget*) const
{
    // TODO
}

QVariant StringDataInformation::dataFromWidget(const QWidget*) const
{
    // TODO
    Q_ASSERT(false);
    return {};
}

QWidget* StringDataInformation::createEditWidget(QWidget*) const
{
    // TODO
    Q_ASSERT(false);
    return nullptr;
}

QString StringDataInformation::typeNameImpl() const
{
    return mData->typeName();
}

unsigned int StringDataInformation::childCount() const
{
    return mData->count();
}

Qt::ItemFlags StringDataInformation::flags(int column, bool fileLoaded) const
{
    return DataInformation::flags(column, fileLoaded);
}

QVariant StringDataInformation::childData(int row, int column, int role) const
{
    Q_ASSERT(row >= 0 && (unsigned)row < childCount());
    Q_ASSERT(column < COLUMN_COUNT);
    if (role == Qt::DisplayRole) {
        if (column == ColumnName) {
            return childNameAt(row);
        }
        if (column == ColumnType) {
            return mData->charType();
        }
        if (column == ColumnValue) {
            return mData->stringValue(row);
        }
        if (column == ColumnSize) {
            return DataInformation::sizeString(childSize(row));
        }
    } else if (role == Qt::ToolTipRole) {
        return DataInformation::tooltipString(childNameAt(row), mData->stringValue(row),
                                              mData->charType(),
                                              DataInformation::sizeString(childSize(row)));
    }
    // TODO mark eof reached, don't add extra item. i.e. add icon or colour
    return {};
}

QString StringDataInformation::valueStringImpl() const
{
    Q_ASSERT(mWasAbleToRead);
    return mData->completeString();
}

Qt::ItemFlags StringDataInformation::childFlags(int row, int column, bool fileLoaded) const
{
    Q_UNUSED(fileLoaded);
    Q_UNUSED(row);
    Q_UNUSED(column);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled; // not editable atm
    // TODO make editable
}

void StringDataInformation::setEncoding(StringDataInformation::StringType encoding)
{
    if (mData && mEncoding == encoding) {
        return;
    }
    if (mData && ((mEncoding == StringType::UTF16_LE && encoding == StringType::UTF16_BE) ||
                  (mEncoding == StringType::UTF16_BE && encoding == StringType::UTF16_LE))) {
        // only set endianess, since is already utf 16
        mData->setEndianness((encoding == StringType::UTF16_LE) ? QSysInfo::LittleEndian : QSysInfo::BigEndian);
    } else if (mData && ((mEncoding == StringType::UTF32_LE && encoding == StringType::UTF32_BE) ||
                         (mEncoding == StringType::UTF32_BE && encoding == StringType::UTF32_LE))) {
        // only set endianess, since is already utf 32
        mData->setEndianness((encoding == StringType::UTF32_LE) ? QSysInfo::LittleEndian : QSysInfo::BigEndian);
    } else {
        StringData* data = nullptr;
        switch (encoding) {
        case StringType::ASCII:
            data = new AsciiStringData(this);
            break;
        case StringType::Latin1:
            data = new Latin1StringData(this);
            break;
        case StringType::UTF8:
            data = new Utf8StringData(this);
            break;
        case StringType::UTF16_LE:
            data = new Utf16StringData(this);
            data->setEndianness(QSysInfo::LittleEndian);
            break;
        case StringType::UTF16_BE:
            data = new Utf16StringData(this);
            data->setEndianness(QSysInfo::BigEndian);
            break;
        case StringType::UTF32_LE:
            data = new Utf32StringData(this);
            data->setEndianness(QSysInfo::LittleEndian);
            break;
        case StringType::UTF32_BE:
            data = new Utf32StringData(this);
            data->setEndianness(QSysInfo::BigEndian);
            break;
        case StringType::EBCDIC:
            data = new EbcdicStringData(this);
            break;
        default:
            data = new AsciiStringData(this);     // TODO add the other classes
            break;
        }
        if (mData) {
            data->copyTerminationFrom(mData.get());
        }
        mData.reset(data);
    }
    mEncoding = encoding;
}
// TODO implement string editing

BitCount32 StringDataInformation::childSize(uint index) const
{
    return mData->sizeAt(index);
}

QString StringDataInformation::childTypeName(uint index) const
{
    Q_UNUSED(index)
    return {}; // XXX should there be something here?
}

QString StringDataInformation::childString(uint index) const
{
    return mData->stringValue(index);
}

void StringDataInformation::setChildWidgetData(uint index, QWidget* w) const
{
    Q_ASSERT(false);
    Q_ASSERT(index < mData->count());
    Q_UNUSED(index)
    Q_UNUSED(w)
}

QVariant StringDataInformation::dataFromChildWidget(uint index, const QWidget* w) const
{
    Q_ASSERT(index < mData->count());
    Q_ASSERT(false);
    Q_UNUSED(w)
    Q_UNUSED(index)
    return {};
}

QWidget* StringDataInformation::createChildEditWidget(uint index, QWidget* parent) const
{
    Q_ASSERT(false);
    Q_UNUSED(parent)
    Q_UNUSED(index)
    return nullptr;
}

QScriptValue StringDataInformation::childToScriptValue(uint index, QScriptEngine*, ScriptHandlerInfo*) const
{
    // just return as a string
    return mData->stringValue(index);
}

BitCount64 StringDataInformation::childPosition(const DataInformation* child, Okteta::Address start) const
{
    Q_ASSERT(child->isDummy());
    Q_ASSERT(child->parent() == this);
    Q_ASSERT(child == &mDummy);
    Q_UNUSED(child)
    uint index = mDummy.dummyIndex();
    Q_ASSERT(index < mData->count());
    BitCount32 offs = 0;
    for (uint i = 0; i < index; ++i) {
        offs += mData->sizeAt(i);
    }

    if (mParent->isTopLevel()) {
        return start * 8 + offs;
    }

    return mParent->asDataInformation()->childPosition(this, start) + offs;
}

QVariant StringDataInformation::data(int column, int role) const
{
    if (mData->wasEof()) {
        if (role == Qt::BackgroundRole) {
            return QBrush(Qt::yellow);
        }
        if (role == Qt::ToolTipRole) {
            return i18n("End of file reached prematurely");
        }
    }
    return DataInformation::data(column, role);
}

bool StringDataInformation::isString() const
{
    return true;
}

void StringDataInformation::unsetTerminationMode(StringData::TerminationMode mode)
{
    // clear the mode and set to null terminated of none is left
    mData->setTerminationMode(StringData::TerminationMode(mData->terminationMode() & ~mode));
    if (mData->terminationMode() == StringData::None) {
        mData->setTerminationCodePoint(0);
    }
}

QScriptClass* StringDataInformation::scriptClass(ScriptHandlerInfo* handlerInfo) const
{
    return handlerInfo->mStringClass.get();
}

bool StringDataInformation::canHaveChildren() const
{
    return true;
}
