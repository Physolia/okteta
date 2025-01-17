/*
    This file is part of the Okteta Kasten module, made within the KDE community.

    SPDX-FileCopyrightText: 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "float32editor.hpp"

// Qt
#include <QDoubleValidator>
#include <QLocale>
// C++
#include <limits>

Float32Editor::Float32Editor(QWidget* parent)
    : QLineEdit(parent)
{
    const float floatHighest = std::numeric_limits<float>::max();
    const float floatLowest = std::numeric_limits<float>::lowest();

    auto* validator = new QDoubleValidator(this);
    validator->setLocale(QLocale::c()); // for consistency with other podtable editors
    validator->setNotation(QDoubleValidator::ScientificNotation);
    validator->setRange(floatLowest, floatHighest, 1000); // 1000 also being default in Qt5

    setValidator(validator);
    setClearButtonEnabled(true);
}

Float32Editor::~Float32Editor() = default;

void Float32Editor::setData(Float32 data)
{
    setText(data.toString());
}

Float32 Float32Editor::data() const
{
//     interpretText();
    return text().toFloat();
}

#include "moc_float32editor.cpp"
