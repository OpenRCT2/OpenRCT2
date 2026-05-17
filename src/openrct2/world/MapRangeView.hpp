/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Location.hpp"

#include <iterator>

namespace OpenRCT2::Map
{
    // Loops over a range from bottomLeft to topRight inclusive
    struct MapRangeView
    {
    private:
        TileCoordsXY _bottomLeft;
        TileCoordsXY _topRight;

        class Iterator
        {
        private:
            const TileCoordsXY _bottomLeft;
            const TileCoordsXY _topRight;
            TileCoordsXY _pos;

        public:
            Iterator(const TileCoordsXY& bottomLeft, const TileCoordsXY& topRight)
                : _bottomLeft(bottomLeft)
                , _topRight(topRight)
                , _pos(bottomLeft)
            {
            }

            Iterator& operator++()
            {
                if (_pos.x >= _topRight.x)
                {
                    _pos.x = _bottomLeft.x;
                    _pos.y++;
                }
                else
                {
                    _pos.x++;
                }
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator retval = *this;
                ++(*this);
                return retval;
            }

            bool operator==(const Iterator& other) const
            {
                return _pos == other._pos;
            }

            const TileCoordsXY& operator*()
            {
                return _pos;
            }
            // iterator traits
            using difference_type = std::ptrdiff_t;
            using value_type = const TileCoordsXY;
            using pointer = TileCoordsXY*;
            using reference = const TileCoordsXY&;
            using iterator_category = std::forward_iterator_tag;
        };

    public:
        MapRangeView(const TileCoordsXY& bottomLeft, const TileCoordsXY& topRight)
            : _bottomLeft(bottomLeft)
            , _topRight(topRight)
        {
            assert(bottomLeft.x <= topRight.x);
            assert(bottomLeft.y <= topRight.y);
        }

        Iterator begin() const
        {
            return Iterator(_bottomLeft, _topRight);
        }
        Iterator end() const
        {
            // End iterator must be 1 step past the end so that loop is inclusive
            return Iterator(TileCoordsXY(_bottomLeft.x, _topRight.y + 1), _topRight);
        }
    };

    MapRangeView getClampedRange(const TileCoordsXY& posA, const TileCoordsXY& posB);
    MapRangeView getClampedRange(const CoordsXY& posA, const CoordsXY& posB);
    MapRangeView getClampedRange(const MapRange& range);
    MapRangeView getDrawableTileRange();
    MapRangeView getWorldRange();
} // namespace OpenRCT2::Map
