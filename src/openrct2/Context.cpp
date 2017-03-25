#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include <exception>
#include "Context.h"
#include "OpenRCT2.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Ui;

namespace OpenRCT2
{
    class Context : public IContext
    {
    private:
        IUiContext * const _uiContext;

    public:
        // Singleton of Context.
        // Remove this when GetContext() is no longer called so that
        // multiple instances can be created in parallel
        static Context * Instance;

    public:
        Context(IUiContext * uiContext)
            : _uiContext(uiContext)
        {
            Instance = this;
        }

        ~Context() override
        {
            Instance = nullptr;
        }

        IUiContext * GetUiContext() override
        {
            return _uiContext;
        }

        sint32 RunOpenRCT2(int argc, char * * argv) override
        {
            core_init();
            int runGame = cmdline_run((const char * *)argv, argc);
            if (runGame == 1)
            {
                openrct2_launch();
            }
            return gExitCode;
        }
    };

    Context * Context::Instance = nullptr;

    IContext * CreateContext()
    {
        return new Context(nullptr);
    }

    IContext * CreateContext(IUiContext * uiContext)
    {
        return new Context(uiContext);
    }

    IContext * GetContext()
    {
        return Context::Instance;
    }
}
