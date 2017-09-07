#ifndef OPENRCT2_INTENT_H
#define OPENRCT2_INTENT_H

#include <map>
#include "../common.h"
#include "../interface/window.h"

#ifdef __cplusplus
class Intent
{
private:
    rct_windowclass _Class;
    std::map<uint32, uint32> _UInts;
    std::map<uint32, sint32> _SInts;
    std::map<uint32, utf8string> _Strings;
    std::map<uint32, uintptr_t> _Pointers;
public:
    explicit Intent(rct_windowclass windowclass);
    Intent * putExtra(uint32 key, uint32 value);
    Intent * putExtra(uint32 key, uintptr_t value);
    Intent * putExtra(uint32 key, sint32 value);
    Intent * putExtra(uint32 key, utf8string value);
};


extern "C" {
#endif // __cplusplus

    typedef int intent;

    intent *intent_create(rct_windowclass clss);
    void intent_set_string(intent *, uint32 key, utf8string value);
    void intent_set_pointer(intent *, uint32 key, uintptr_t value);
    void intent_set_sint(intent *, uint32 key, sint32 value);
    void intent_set_uint(intent *, uint32 key, uint32 value);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // OPENRCT2_INTENT_H
