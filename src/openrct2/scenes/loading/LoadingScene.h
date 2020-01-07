/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../common.h"
#include "../../core/JobPool.hpp"
#include "../../drawing/Drawing.h"
#include "../Scene.h"

namespace OpenRCT2
{
    class LoadingScene final : public Scene
    {
    public:
        LoadingScene(IContext& context);

        void Load() override;
        void Update() override;
        void Stop() override;
        void AddJob(const std::function<void()>& fn)
        {
            _jobs.AddTask(fn);
        }

    private:
        JobPool _jobs;
    };
} // namespace OpenRCT2
