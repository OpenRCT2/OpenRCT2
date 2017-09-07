#include "Intent.h"

Intent::Intent(rct_windowclass windowclass)
{
this->_Class = windowclass;
}

Intent *
Intent::putExtra(uint32 key, uint32 value)
{
    _UInts.insert(std::make_pair(key, value));

    return this;
}
Intent *
Intent::putExtra(uint32 key, uintptr_t value)
{
    _Pointers.insert(std::make_pair(key, value));

    return this;
}
Intent *
Intent::putExtra(uint32 key, sint32 value)
{
    _Pointers.insert(std::make_pair(key, value));

    return this;
}
Intent *
Intent::putExtra(uint32 key, utf8string value)
{
    _Strings.insert(std::make_pair(key, value));

    return this;
}
