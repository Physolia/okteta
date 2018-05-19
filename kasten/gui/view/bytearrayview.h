/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2006-2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef BYTEARRAYVIEW_H
#define BYTEARRAYVIEW_H

// lib
#include <kasten/okteta/oktetakastengui_export.h>
// lib
#include <kasten/okteta/bytearrayselection.h>
// Kasten gui
#include <kasten/abstractview.h>
#include <kasten/zoomable.h> // TODO: able instead of interface? or else?
#include <kasten/dataselectable.h>
#include <kasten/selecteddatawriteable.h>
// Okteta core
#include <okteta/addressrange.h>

namespace Okteta {
class ByteArrayJanusView;
}
class QRect;

namespace Kasten {
class ByteArrayViewProfileSynchronizer;
class ByteArrayDocument;
class KCursorProxy;

class OKTETAKASTENGUI_EXPORT ByteArrayView : public AbstractView
                                           , public If::Zoomable
                                           , public If::DataSelectable
                                           , public If::SelectedDataWriteable
{
    Q_OBJECT
    Q_INTERFACES(
        Kasten::If::Zoomable
        Kasten::If::DataSelectable
        Kasten::If::SelectedDataWriteable
    )

public:
    enum
    {
        ColumnViewId = 0,
        RowViewId
    };

public:
    ByteArrayView(ByteArrayDocument* document, ByteArrayViewProfileSynchronizer* synchronizer);
    ByteArrayView(ByteArrayView* other, ByteArrayViewProfileSynchronizer* synchronizer,
                  Qt::Alignment alignment = {});

    ~ByteArrayView() override;

public:
//     KCursorObject *cursor() const;

public: // AbstractModel API
    QString title() const override;
    bool isModifiable() const override;
    bool isReadOnly() const override;
    void setReadOnly(bool isReadOnly) override;

public: // AbstractView API
    void setFocus() override;
    QWidget* widget() const override;
    bool hasFocus() const override;

public: // If::Zoomable API TODO: make all such methods slots?
    void setZoomLevel(double Level) override;
    double zoomLevel() const override;
Q_SIGNALS:
    void zoomLevelChanged(double Level) override;

public: // If::DataSelectable API
    void selectAllData(bool selectAll) override;
    bool hasSelectedData() const override;
    QMimeData* copySelectedData() const override;
    const AbstractModelSelection* modelSelection() const override;
//     virtual void setSelection();
Q_SIGNALS:
    void hasSelectedDataChanged(bool hasSelectedData) override;
    void selectedDataChanged(const Kasten::AbstractModelSelection* modelSelection) override;

public: // If::SelectedDataWriteable API
    void insertData(const QMimeData* data) override;
    QMimeData* cutSelectedData() override;
    void deleteSelectedData() override;
    bool canReadData(const QMimeData* data) const override;

public: // cursor API
    void setCursorPosition(Okteta::Address cursorPosition);
    void setSelectionCursorPosition(Okteta::Address index);
    Okteta::Address cursorPosition() const;
Q_SIGNALS:
    void cursorPositionChanged(Okteta::Address cursorPosition);

public: // codings
    void setValueCoding(int valueCoding);
    void setCharCoding(const QString& charCodingName);
    QString charCodingName() const;
    int valueCoding() const;
Q_SIGNALS:
    void charCodecChanged(const QString& charCodingName);
    void valueCodingChanged(int valueCoding);

public:
    // TODO: see how this can be solved by modelSelection
    Okteta::AddressRange selection() const;
    void setSelection(Okteta::Address start, Okteta::Address end);
    void insert(const QByteArray& byteArray);
    QRect cursorRect() const;

public: // overwrite
    void setOverwriteMode(bool overwriteMode);
    bool isOverwriteMode() const;
    bool isOverwriteOnly() const;
Q_SIGNALS:
    void overwriteModeChanged(bool overwriteMode);

public: // elements
    void toggleOffsetColumn(bool visible);
    void setOffsetCoding(int offsetCoding);
    void setVisibleByteArrayCodings(int columns);
    void setMarking(const Okteta::AddressRange& range, bool ensureVisible = false);
    bool offsetColumnVisible() const;
    int offsetCoding() const;
    int visibleByteArrayCodings() const;
Q_SIGNALS:
    void offsetColumnVisibleChanged(bool visible);
    void visibleByteArrayCodingsChanged(int columns);
    void offsetCodingChanged(int offsetCoding);

public: // table layout
    void setLayoutStyle(int layoutStyle);
    void setNoOfBytesPerLine(int noOfBytesPerLine);
    Okteta::Address startOffset() const;
    Okteta::Address firstLineOffset() const;
    int noOfBytesPerLine() const;
    int layoutStyle() const;
Q_SIGNALS:
    void layoutStyleChanged(int layoutStyle);
    void noOfBytesPerLineChanged(int noOfBytesPerLine);

public: // layout settings
    void setShowsNonprinting(bool showsNonprinting = true);
    void setNoOfGroupedBytes(int noOfGroupedBytes);
    void setSubstituteChar(QChar substituteChar);
    void setUndefinedChar(QChar undefinedChar);
    QChar substituteChar() const;
    QChar undefinedChar() const;
    bool showsNonprinting() const;

    int byteSpacingWidth() const;
    int noOfGroupedBytes() const;
    int groupSpacingWidth() const;
    int binaryGapWidth() const;
Q_SIGNALS:
    void showsNonprintingChanged(bool showsNonprinting);
    void substituteCharChanged(QChar substituteChar);
    void undefinedCharChanged(QChar undefinedChar);
    void noOfGroupedBytesChanged(int noOfGroupedBytes);

public:
    void setViewModus(int viewModus);
    int viewModus() const;
Q_SIGNALS:
    void viewModusChanged(int viewModus);

public:
    ByteArrayViewProfileSynchronizer* synchronizer() const;

private:
    void init();

private Q_SLOTS:
    void setFontByGlobalSettings();

    void onSelectionChanged(const Okteta::AddressRange& selection);

private:
    Okteta::ByteArrayJanusView* mWidget;
    ByteArrayDocument* mDocument;
    ByteArraySelection mSelection;
//     KCursorProxy *mCursorProxy;

    ByteArrayViewProfileSynchronizer* const mByteArrayViewProfileSynchronizer;
};

}

#endif
