/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    SPDX-FileCopyrightText: 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "poddecodertoolview.hpp"

// tool
#include "podtableview.hpp"
#include "poddecodertool.hpp"

namespace Kasten {

PODDecoderToolView::PODDecoderToolView(PODDecoderTool* tool, AbstractUserMessagesHandler* userMessagesHandler)
    : mWidget(std::make_unique<PODTableView>(tool, userMessagesHandler))
{
}

PODDecoderToolView::~PODDecoderToolView() = default;

QWidget* PODDecoderToolView::widget()    const { return mWidget.get(); }
QString PODDecoderToolView::title()      const { return mWidget->tool()->title(); }
AbstractTool* PODDecoderToolView::tool() const { return mWidget->tool(); }

}

#include "moc_poddecodertoolview.cpp"
