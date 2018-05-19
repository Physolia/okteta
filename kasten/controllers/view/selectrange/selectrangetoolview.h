/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef SELECTRANGETOOLVIEW_H
#define SELECTRANGETOOLVIEW_H

// Kasten gui
#include <kasten/abstracttoolinlineview.h>

namespace Kasten {

class SelectRangeView;
class SelectRangeTool;

class SelectRangeToolView : public AbstractToolInlineView
{
public:
    explicit SelectRangeToolView(SelectRangeTool* tool);
    ~SelectRangeToolView() override;

public: // AbstractToolView API
    QWidget* widget() const override;
    QString title() const override;
    AbstractTool* tool() const override;

private:
    SelectRangeView* mWidget;
};

}

#endif
