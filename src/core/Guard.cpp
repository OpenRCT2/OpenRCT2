#include <cassert>
#include <stdio.h>

#include "Console.hpp"
#include "Diagnostics.hpp"
#include "Guard.hpp"

namespace Guard
{
    void Assert(bool expression, const char * message)
    {
        if (expression) return;

        if (message != nullptr)
        {
            Console::Error::WriteLine(message);
        }

#if DEBUG
        Debug::Break();
#endif
        assert(false);
    }
}
