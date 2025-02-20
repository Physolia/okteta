/*
    This file is part of the Kasten Framework, made within the KDE community.

    SPDX-FileCopyrightText: 2007-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef KASTEN_ABSTRACTDOCUMENT_P_HPP
#define KASTEN_ABSTRACTDOCUMENT_P_HPP

// lib
#include "abstractmodel_p.hpp"
#include "abstractdocument.hpp"
#include "abstractmodelsynchronizer.hpp"
// Std
#include <memory>

namespace Kasten {

class AbstractDocumentPrivate : public AbstractModelPrivate
{
public:
    explicit AbstractDocumentPrivate(AbstractDocument* parent);

    ~AbstractDocumentPrivate() override;

public:
    [[nodiscard]]
    const QString& id() const;
    [[nodiscard]]
    AbstractModelSynchronizer* synchronizer() const;

public:
    void setId(const QString& id);
    void setSynchronizer(std::unique_ptr<AbstractModelSynchronizer>&& synchronizer);

private:
    Q_DECLARE_PUBLIC(AbstractDocument)

private:
    QString mId;
    std::unique_ptr<AbstractModelSynchronizer> mSynchronizer; // TODO: should this be here, with public setters and getters?
};

inline AbstractDocumentPrivate::AbstractDocumentPrivate(AbstractDocument* parent)
    : AbstractModelPrivate(parent)
{}

inline AbstractDocumentPrivate::~AbstractDocumentPrivate() = default;

inline const QString& AbstractDocumentPrivate::id() const { return mId; }
inline void AbstractDocumentPrivate::setId(const QString& id) { mId = id; }

inline AbstractModelSynchronizer* AbstractDocumentPrivate::synchronizer() const { return mSynchronizer.get(); }
inline void AbstractDocumentPrivate::setSynchronizer(std::unique_ptr<AbstractModelSynchronizer>&& synchronizer)
{
    Q_Q(AbstractDocument);

    // plugging the same more than once?
    if (mSynchronizer == synchronizer) {
        return;
    }

    mSynchronizer = std::move(synchronizer);

    Q_EMIT q->synchronizerChanged(mSynchronizer.get());
}

}

#endif
