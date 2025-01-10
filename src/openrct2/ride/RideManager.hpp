/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Ride.h"

namespace OpenRCT2
{
    struct RideManager
    {
    private:
        GameState_t& _gameState;

    public:
        RideManager();
        RideManager(GameState_t& gameState);

        const Ride* operator[](RideId id) const
        {
            return GetRide(id);
        }

        Ride* operator[](RideId id)
        {
            return GetRide(id);
        }

        class Iterator
        {
            friend RideManager;

        private:
            RideManager* _rideManager;
            RideId::UnderlyingType _index{};
            RideId::UnderlyingType _endIndex{};

        public:
            using difference_type = intptr_t;
            using value_type = Ride;
            using pointer = const Ride*;
            using reference = const Ride&;
            using iterator_category = std::forward_iterator_tag;

        private:
            Iterator(RideManager& rideManager, size_t beginIndex, size_t endIndex)
                : _rideManager(&rideManager)
                , _index(static_cast<RideId::UnderlyingType>(beginIndex))
                , _endIndex(static_cast<RideId::UnderlyingType>(endIndex))
            {
                if (_index < _endIndex && (*_rideManager)[RideId::FromUnderlying(_index)] == nullptr)
                {
                    ++(*this);
                }
            }

        public:
            Iterator& operator++()
            {
                do
                {
                    _index++;
                } while (_index < _endIndex && (*_rideManager)[RideId::FromUnderlying(_index)] == nullptr);
                return *this;
            }
            Iterator operator++(int)
            {
                auto result = *this;
                ++(*this);
                return result;
            }
            bool operator==(Iterator other) const
            {
                return _index == other._index;
            }
            bool operator!=(Iterator other) const
            {
                return !(*this == other);
            }
            Ride& operator*()
            {
                return *(*_rideManager)[RideId::FromUnderlying(_index)];
            }
        };

        size_t size() const;
        Iterator begin();
        Iterator end();
        Iterator get(RideId rideId);
        Iterator begin() const
        {
            return (const_cast<RideManager*>(this))->begin();
        }
        Iterator end() const
        {
            return (const_cast<RideManager*>(this))->end();
        }
    };

    RideManager GetRideManager();
} // namespace OpenRCT2
