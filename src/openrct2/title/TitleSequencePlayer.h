/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

namespace OpenRCT2
{
    struct ITitleSequencePlayer
    {
        virtual ~ITitleSequencePlayer() = default;

        virtual int32_t GetCurrentPosition() const abstract;

        virtual bool Begin(size_t titleSequenceId) abstract;
        virtual void Reset() abstract;
        virtual bool Update() abstract;
        virtual void Seek(int32_t position) abstract;
        virtual void Eject() abstract;
    };
} // namespace OpenRCT2
