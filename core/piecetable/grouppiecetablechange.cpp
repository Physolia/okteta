/*
    This file is part of the Okteta Core library, part of the KDE project.

    Copyright 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "grouppiecetablechange.h"


// lib
#include "piecetable.h"
//
#include <ksectionlist.h>
#include <arraychangemetricslist.h>
// KDE
#include <KLocale>


namespace KPieceTable
{

int GroupPieceTableChange::type() const { return GroupId; }

QString GroupPieceTableChange::description() const
{
    return mDescription;
}

bool GroupPieceTableChange::merge( const AbstractPieceTableChange *other )
{
    bool result = false;

    if( !mChangeList.isEmpty() )
        result = mChangeList.last()->merge( other );

    return result;
}
#if 0
KHE::KSection GroupPieceTableChange::apply( PieceTable *pieceTable ) const
{
    pieceTable->insert( mInsertOffset, mInsertLength, mStorageOffset );

    return KHE::KSection( mInsertOffset, pieceTable->size()-1 );
}

KHE::KSection GroupPieceTableChange::revert( PieceTable *pieceTable ) const
{
//     const int oldLast = pieceTable->size() - 1;
//     pieceTable->remove( KHE::KSection::fromWidth(mInsertOffset,mInsertLength) );
    return KHE::KSection();//( mInsertOffset, oldLast );
}

KHE::ArrayChangeMetrics GroupPieceTableChange::metrics() const
{
    return KHE::ArrayChangeMetrics;//::asReplacement( mInsertOffset, 0, mInsertLength );
}
#endif
KHE::KSectionList GroupPieceTableChange::applyGroup( PieceTable *pieceTable ) const
{
    KHE::KSectionList result;
    foreach( AbstractPieceTableChange *change, mChangeList )
    {
        if( change->type() == AbstractPieceTableChange::GroupId )
        {
            const GroupPieceTableChange *groupChange = static_cast<const GroupPieceTableChange *>(change);
            const KHE::KSectionList changedSectionList = groupChange->applyGroup( pieceTable );
            result.addSectionList( changedSectionList );
        }
        else
            result.append( change->apply(pieceTable) );
    }

    return result;
}

KHE::KSectionList GroupPieceTableChange::revertGroup( PieceTable *pieceTable ) const
{
    KHE::KSectionList result;
    foreach( AbstractPieceTableChange *change, mChangeList )
    {
        if( change->type() == AbstractPieceTableChange::GroupId )
        {
            const GroupPieceTableChange *groupChange = static_cast<const GroupPieceTableChange *>(change);
            const KHE::KSectionList changedSectionList = groupChange->revertGroup( pieceTable );
            result.addSectionList( changedSectionList );
        }
        else
            result.append( change->revert(pieceTable) );
    }

    return result;
}

KHE::ArrayChangeMetricsList GroupPieceTableChange::groupMetrics() const
{
    KHE::ArrayChangeMetricsList result;
    foreach( AbstractPieceTableChange *change, mChangeList )
    {
        if( change->type() == AbstractPieceTableChange::GroupId )
        {
            const GroupPieceTableChange *groupChange = static_cast<const GroupPieceTableChange *>(change);
            const KHE::ArrayChangeMetricsList metricsList = groupChange->groupMetrics();
            result += metricsList;
        }
        else
            result.append( change->metrics() );
    }

    return result;
}


int GroupPieceTableChange::dataSize() const
{
    int result = 0;
    foreach( AbstractPieceTableChange *change, mChangeList )
        result += change->dataSize();
    return result;
 }

GroupPieceTableChange::~GroupPieceTableChange() {}

}
