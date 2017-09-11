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
Intent::putExtra(uint32 key, void * value)
{
    _Pointers.insert(std::make_pair(key, (uintptr_t) value));

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
rct_windowclass
Intent::GetWindowClass()
{
    return this->_Class;
}
void *
Intent::GetPointerExtra(uint32 key)
{
    return (void *) _Pointers.at(key);
}
uint32
Intent::GetUIntExtra(uint32 key)
{
    return _UInts.at(key);
}
sint32
Intent::GetSIntExtra(uint32 key)
{
    return _SInts.at(key);
}
utf8string
Intent::GetStringExtra(uint32 key)
{
    return _Strings.at(key);
}

extern "C" {
    Intent *intent_create(rct_windowclass clss)
    {
        return new Intent(clss);
    }

    void intent_release(Intent *intent)
    {
        delete intent;
    }

    void intent_set_sint(Intent * intent, uint32 key, sint32 value)
    {
        intent->putExtra(key, value);
    }

    void intent_set_string(Intent *intent, uint32 key, utf8string value)
    {
        intent->putExtra(key, value);
    }

    void intent_set_pointer(Intent *intent, uint32 key, void *value)
    {
        intent->putExtra(key, value);
    }

    void intent_set_uint(Intent *intent, uint32 key, uint32 value)
    {
        intent->putExtra(key, value);
    }
}

