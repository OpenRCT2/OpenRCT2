/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once 

#include "TitleSequencePlayer.h"
#include <memory>

class DummyTitleSequencePlayer final : public ITitleSequencePlayer{
    public: 
        DummyTitleSequencePlayer();

        virtual ~DummyTitleSequencePlayer() = default;
        int32_t GetCurrentPosition() const override;

        bool Begin(size_t titleSequenceId) override;
        void Reset() override;
        bool Update() override;
        void Seek(int32_t position) override;
        void Eject() override;
};
