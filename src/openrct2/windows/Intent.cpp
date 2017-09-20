#include "Intent.h"

Intent::Intent(rct_windowclass windowclass)
{
    this->_Class = windowclass;
}

Intent * Intent::putExtra(uint32 key, uint32 value)
{
    IntentData data = {};
    data.uintVal = value;
    data.type = IntentData::DT_UINT;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32 key, void * value)
{
    IntentData data = {};
    data.pointerVal = value;
    data.type = IntentData::DT_POINTER;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32 key, sint32 value)
{
    IntentData data = {};
    data.sintVal = value;
    data.type = IntentData::DT_SINT;

    _Data.insert(std::make_pair(key, data));

    return this;
}

Intent * Intent::putExtra(uint32 key, utf8string value)
{
    IntentData data = {};
    data.stringVal = value;
    data.type = IntentData::DT_STRING;

    _Data.insert(std::make_pair(key, data));

    return this;
}

rct_windowclass Intent::GetWindowClass()
{
    return this->_Class;
}

void * Intent::GetPointerExtra(uint32 key)
{
    auto data =  _Data.at(key);
    return (void *) data.pointerVal;
}

uint32 Intent::GetUIntExtra(uint32 key)
{
    auto data =  _Data.at(key);
    return data.uintVal;
}

sint32 Intent::GetSIntExtra(uint32 key)
{
    auto data =  _Data.at(key);
    return data.sintVal;
}

utf8string Intent::GetStringExtra(uint32 key)
{
    auto data =  _Data.at(key);
    return data.stringVal;
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

