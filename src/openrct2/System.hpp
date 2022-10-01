/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

namespace OpenRCT2
{
    struct IContext;

    // Base class for systems that are hosted in the Context.
    class System
    {
        IContext& _context;

    public:
        System(IContext& owner)
            : _context(owner)
        {
        }

        virtual ~System() = default;

        // Called before a tick.
        virtual void PreTick()
        {
        }

        // Called every game tick, which by default is 40hz.
        virtual void Tick()
        {
        }

        // Called after a tick.
        virtual void PostTick()
        {
        }

        // Called every frame.
        virtual void Update()
        {
        }

        IContext& GetContext()
        {
            return _context;
        }

        const IContext& GetContext() const
        {
            return _context;
        }
    };

} // namespace OpenRCT2
