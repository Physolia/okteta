/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    SPDX-FileCopyrightText: 2006-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "podtableview.hpp"

// controller
#include "podtablemodel.hpp"
#include "poddelegate.hpp"
#include "poddecodertool.hpp"
// utils
#include <labelledtoolbarwidget.hpp>
// KF
#include <KComboBox>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>
// Qt
#include <QToolBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QTreeView>
#include <QHeaderView>
#include <QMenu>
#include <QMimeData>
#include <QClipboard>
#include <QApplication>
#include <QFocusEvent>
#include <QFontMetrics>

namespace Kasten {

PODTableView::PODTableView(PODDecoderTool* tool, QWidget* parent)
    : QWidget(parent)
    , mTool(tool)
{
    auto* baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(0, 0, 0, 0);
    baseLayout->setSpacing(0);

    // table
    mPODTableModel = new PODTableModel(mTool, this);
    mPODTableView = new QTreeView(this);
    mPODTableView->setObjectName(QStringLiteral("PODTable"));
    mPODTableView->setRootIsDecorated(false);
    mPODTableView->setAlternatingRowColors(true);
    mPODTableView->setItemsExpandable(false);
    mPODTableView->setUniformRowHeights(true);
    mPODTableView->setAllColumnsShowFocus(true);
    mPODTableView->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
    mPODTableView->setDragEnabled(true);
    mPODTableView->setSortingEnabled(false);
    mPODTableView->setModel(mPODTableModel);
    mPODTableView->installEventFilter(this);
    mPODTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    mPODDelegate = new PODDelegate(mTool, this);
    mPODTableView->setItemDelegate(mPODDelegate);
    QHeaderView* header = mPODTableView->header();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setStretchLastSection(false);
    connect(mPODTableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this, &PODTableView::onCurrentRowChanged);
    connect(mPODTableView, &QWidget::customContextMenuRequested,
            this, &PODTableView::onCustomContextMenuRequested);

    baseLayout->addWidget(mPODTableView, 10);

    // settings
    auto* settingsToolBar = new QToolBar(this);

    mByteOrderSelection = new KComboBox(this);
    mByteOrderSelection->addItem(i18nc("@item:inlistbox", "Big-endian"));     // add first for index
    mByteOrderSelection->addItem(i18nc("@item:inlistbox", "Little-endian"));  // add second for index
    mByteOrderSelection->setCurrentIndex(mTool->byteOrder());
    connect(mByteOrderSelection, qOverload<int>(&KComboBox::activated),
            mTool, &PODDecoderTool::setByteOrder);
    const QString byteOrderToolTip =
        i18nc("@info:tooltip",
              "The byte order to use for decoding the bytes.");
    mByteOrderSelection->setToolTip(byteOrderToolTip);
    settingsToolBar->addWidget(mByteOrderSelection);

    auto* unsignedAsHexLabel = new QLabel(i18nc("@option:check", "Unsigned as hexadecimal:"), this);

    mUnsignedAsHexCheck = new QCheckBox(this);
    mUnsignedAsHexCheck->setChecked(mTool->isUnsignedAsHex());
    connect(mUnsignedAsHexCheck, &QCheckBox::toggled,
            mTool, &PODDecoderTool::setUnsignedAsHex);
    auto* labelledUnsignedAsHexCheck = new LabelledToolBarWidget(unsignedAsHexLabel, mUnsignedAsHexCheck, this);
    const QString unsignedAsHexToolTip =
        i18nc("@info:tooltip",
              "Sets whether the values of the unsigned integer types are shown as hexadecimal instead of as decimal.");
    unsignedAsHexLabel->setToolTip(unsignedAsHexToolTip);
    mUnsignedAsHexCheck->setToolTip(unsignedAsHexToolTip);
    settingsToolBar->addWidget(labelledUnsignedAsHexCheck);

    baseLayout->addWidget(settingsToolBar);

    mTool->setDifferentSizeDialog(this);

    // resize to fit width of contents
    // this is much (!) faster than using setResizeMode(QHeaderView::ResizeToContents)
    QFont f;
    QFontMetrics metrics(f);
    // ideally we should check the width of the longest translated string, but this should be wide enough for most
    // anyway this is just an initial setting and the width can be changed manually
    header->resizeSection(0, metrics.horizontalAdvance(QStringLiteral("Hexadecimal 8-bit")) + 30);
    header->resizeSection(1, metrics.horizontalAdvance(QStringLiteral("1.01234567890123456789e-111")) + 15);
}

PODTableView::~PODTableView() = default;

Answer PODTableView::query(int newValueSize, int oldValueSize, int sizeLeft)
{
    Q_UNUSED(sizeLeft);

    Answer answer;

    int messageBoxAnswer;
    if (newValueSize < oldValueSize) {
        const QString message =
            xi18nc("@info",
                   "The new value needs <emphasis>fewer</emphasis> bytes (%1 instead of %2).<nl/>"
                   "Keep the unused bytes or remove them?", newValueSize, oldValueSize);

        const KGuiItem keepGuiItem =
            KGuiItem(i18nc("@action:button keep the unused bytes",
                           "&Keep"),
                     QString(),
                     i18nc("@info:tooltip",
                           "Keep the unused bytes with their old values."));

        messageBoxAnswer = KMessageBox::warningTwoActionsCancel(this,
                                                                message, mTool->title(),
                                                                keepGuiItem,
                                                                KStandardGuiItem::remove());
    } else {
        const QString message =
            xi18nc("@info",
                   "The new value needs <emphasis>more</emphasis> bytes (%1 instead of %2).<nl/>"
                   "Overwrite the following bytes or insert new ones as needed?", newValueSize, oldValueSize);

        messageBoxAnswer = KMessageBox::warningTwoActionsCancel(this,
                                                                message, mTool->title(),
                                                                KStandardGuiItem::overwrite(),
                                                                KStandardGuiItem::insert());
    }

    answer = (messageBoxAnswer == KMessageBox::PrimaryAction) ?   Overwrite :
             (messageBoxAnswer == KMessageBox::SecondaryAction) ? AdaptSize :
                                                                  Cancel;
    return answer;
}

void PODTableView::selectBytesInView()
{
    auto* action = static_cast<QAction*>(sender());
    const int podId = action->data().toInt();

    mTool->selectBytesInView(podId);
}

void PODTableView::copyToClipboard()
{
    auto* action = static_cast<QAction*>(sender());
    const int podId = action->data().toInt();

    auto* mimeData = new QMimeData;

    const QString displayText = mPODDelegate->displayText(mTool->value(podId), mPODTableView->locale());
    mimeData->setText(displayText);
    mimeData->setData(QStringLiteral("application/octet-stream"), mTool->bytes(podId));

    QApplication::clipboard()->setMimeData(mimeData);
}

bool PODTableView::eventFilter(QObject* object, QEvent* event)
{
    if (object == mPODTableView) {
        if (event->type() == QEvent::FocusIn) {
            const QModelIndex current = mPODTableView->selectionModel()->currentIndex();
            const int podId = current.row();
            if (current.isValid() && mTool->isApplyable() && !mTool->value(podId).isNull()) {
                mTool->markPOD(podId);
            }
        } else if (event->type() == QEvent::FocusOut) {
            QWidget* tableViewFocusWidget = mPODTableView->focusWidget();
            const bool subChildHasFocus = (tableViewFocusWidget != mPODTableView);
            if (subChildHasFocus) {
                mPODTableViewFocusChild = tableViewFocusWidget;
                mPODTableViewFocusChild->installEventFilter(this);
            } else if (mTool->isApplyable()) {
                mTool->unmarkPOD();
            }
        }
    } else if (object == mPODTableViewFocusChild) {
        // TODO: it is only assumed the edit widget will be removed if it loses the focus
        if (event->type() == QEvent::FocusOut) {
            if (!mPODTableView->hasFocus() && mTool->isApplyable()) {
                mTool->unmarkPOD();
            }
            mPODTableViewFocusChild->removeEventFilter(this);
            mPODTableViewFocusChild = nullptr;
        }
    }

    return QWidget::eventFilter(object, event);
}

void PODTableView::onCustomContextMenuRequested(QPoint pos)
{
    const QModelIndex index = mPODTableView->indexAt(pos);
    if (!index.isValid()) {
        return;
    }
    const int podId = index.row();
    if (mTool->value(podId).isNull()) {
        return;
    }

    auto* menu = new QMenu(this);

    // TODO: split into explicit "Copy As Data" and "Copy As Text"
    auto* copyAction =  KStandardAction::copy(this, &PODTableView::copyToClipboard,  this);
    copyAction->setShortcut(QKeySequence());
    copyAction->setData(podId);
    menu->addAction(copyAction);

    // TODO: reusing string due to string freeze
    auto* selectAction = new QAction(i18nc("@action:button", "&Select"), this);
    connect(selectAction, &QAction::triggered,
            this, &PODTableView::selectBytesInView);
    selectAction->setData(podId);
    menu->addAction(selectAction);

    menu->popup(mPODTableView->viewport()->mapToGlobal(pos));
}

void PODTableView::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous)
{
    Q_UNUSED(previous)

    if (!mTool->isApplyable()) {
        return;
    }

    const int podId = current.row();
    if (current.isValid() && !mTool->value(podId).isNull()) {
        mTool->markPOD(podId);
    } else {
        mTool->unmarkPOD();
    }
}

}

#include "moc_podtableview.cpp"
