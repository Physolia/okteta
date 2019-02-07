/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2006,2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef VIEWMANAGER_HPP
#define VIEWMANAGER_HPP

// lib
#include <kasten/abstractview.hpp>
// Qt
#include <QVector>
#include <QObject>

namespace Kasten {

class ModelCodecViewManager;
class AbstractViewFactory;

class KASTENGUI_EXPORT ViewManager : public QObject
{
    Q_OBJECT

public:
    ViewManager();

    ~ViewManager() override;

public:
    void setViewFactory(AbstractViewFactory* factory);

    void createCopyOfView(AbstractView* view, Qt::Alignment alignment = {});
    void removeViews(const QVector<AbstractView*>& views);

public:
    QVector<AbstractView*> views() const;
    AbstractView* viewByWidget(QWidget* widget) const;

public:
    ModelCodecViewManager* codecViewManager();

public Q_SLOTS:
    void createViewsFor(const QVector<Kasten::AbstractDocument*>& documents);
    void removeViewsFor(const QVector<Kasten::AbstractDocument*>& documents);

Q_SIGNALS:
    // view was created and already added to the list
    void opened(const QVector<Kasten::AbstractView*>& views);
    // view will be closed, already removed from list
    void closing(const QVector<Kasten::AbstractView*>& views);

private:
    QVector<AbstractView*> mViewList;
    AbstractViewFactory* mFactory = nullptr;

    // TODO: remove into own singleton
    ModelCodecViewManager* mCodecViewManager;
};

inline ModelCodecViewManager* ViewManager::codecViewManager() { return mCodecViewManager; }

}

#endif