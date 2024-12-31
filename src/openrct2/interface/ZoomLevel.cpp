/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ZoomLevel.h"

ZoomLevel ZoomLevel::operator++(int)
{
    ZoomLevel tmp(*this);
    operator++();
    return tmp;
}

ZoomLevel& ZoomLevel::operator++()
{
    _level++;
    return *this;
}

ZoomLevel ZoomLevel::operator--(int)
{
    ZoomLevel tmp(*this);
    operator--();
    return tmp;
}

ZoomLevel& ZoomLevel::operator--()
{
    _level--;
    return *this;
}

ZoomLevel& ZoomLevel::operator=(const ZoomLevel& other)
{
    _level = other._level;
    return *this;
}

ZoomLevel& ZoomLevel::operator+=(const ZoomLevel& rhs)
{
    _level += rhs._level;
    return *this;
}

ZoomLevel& ZoomLevel::operator-=(const ZoomLevel& rhs)
{
    _level -= rhs._level;
    return *this;
}

ZoomLevel operator+(ZoomLevel lhs, const ZoomLevel& rhs)
{
    lhs += rhs;
    return lhs;
}

ZoomLevel operator-(ZoomLevel lhs, const ZoomLevel& rhs)
{
    lhs -= rhs;
    return lhs;
}

ZoomLevel operator+(ZoomLevel lhs, int8_t rhs)
{
    lhs += ZoomLevel{ rhs };
    return lhs;
}

ZoomLevel operator-(ZoomLevel lhs, int8_t rhs)
{
    lhs -= ZoomLevel{ rhs };
    return lhs;
}
