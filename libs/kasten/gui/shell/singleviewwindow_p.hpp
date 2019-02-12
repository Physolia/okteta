/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2011-2012 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef KASTEN_SINGLEVIEWWINDOW_P_HPP
#define KASTEN_SINGLEVIEWWINDOW_P_HPP

// lib
#include "singleviewwindow.hpp"
// Kasten core
#include <Kasten/KastenCore>
// Qt
#include <QVector>

class QDragMoveEvent;
class QDropEvent;

namespace Kasten {

class SingleViewArea;
class AbstractTool;
class AbstractModelSynchronizer;
class AbstractDocument;

class SingleViewWindowPrivate
{
public:
    SingleViewWindowPrivate(SingleViewWindow* parent,
                            AbstractView* view);

    ~SingleViewWindowPrivate();

public:
    void init();

public:
    void setView(AbstractView* view);
    void addXmlGuiController(AbstractXmlGuiController* controller);
    void addTool(AbstractToolView* toolView);

public: // If::WidgetsDockable API
    QVector<ToolViewDockWidget*> dockWidgets() const;

private:
    AbstractView* view() const;
    SingleViewArea* viewArea() const;

private: // Q_SLOTS
    void onTitleChanged(const QString& newTitle);
    void onContentFlagsChanged(Kasten::ContentFlags contentFlags);
    void onLocalSyncStateChanged(Kasten::LocalSyncState newState);
    void onToolVisibilityChanged(bool isVisible);
    void onSynchronizerDeleted(QObject* synchronizer);

private:
    Q_DECLARE_PUBLIC(SingleViewWindow)

private:
    SingleViewWindow* const q_ptr;

    AbstractView* mView = nullptr;
    AbstractDocument* mDocument = nullptr;
    AbstractModelSynchronizer* mSynchronizer = nullptr;

    SingleViewArea* const mViewArea;
    QVector<AbstractXmlGuiController*> mControllers;
    QVector<ToolViewDockWidget*> mDockWidgets;
    QVector<AbstractTool*> mTools;
};

inline QVector<ToolViewDockWidget*> SingleViewWindowPrivate::dockWidgets() const { return mDockWidgets; }
inline AbstractView* SingleViewWindowPrivate::view() const { return mView; }
inline SingleViewArea* SingleViewWindowPrivate::viewArea() const { return mViewArea; }

}

#endif
