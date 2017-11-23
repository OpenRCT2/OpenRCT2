#ifndef OPENRCT2_INTENT_H
#define OPENRCT2_INTENT_H

#include "../common.h"
#include "../interface/window.h"
#ifdef __cplusplus
#include <map>
#include <string>
#endif // __cplusplus


#ifdef __cplusplus
struct IntentData
{
    enum DATATYPE { DT_INT, DT_STRING, DT_POINTER, DT_CLOSE_CALLBACK } type;

    union {
        uint32 unsignedInt;
        sint32 signedInt;
    } intVal;
    std::string stringVal;
    close_callback closeCallbackVal;
    void * pointerVal;
};

class Intent
{
private:
    rct_windowclass _Class;
    std::map<uint32, IntentData> _Data;
public:
    explicit Intent(rct_windowclass windowclass);
    rct_windowclass GetWindowClass() const;
    void * GetPointerExtra(uint32 key);
    std::string GetStringExtra(uint32 key);
    uint32 GetUIntExtra(uint32 key);
    sint32 GetSIntExtra(uint32 key);
    close_callback GetCloseCallbackExtra(uint32 key);
    Intent * putExtra(uint32 key, uint32 value);
    Intent * putExtra(uint32 key, void * value);
    Intent * putExtra(uint32 key, sint32 value);
    Intent * putExtra(uint32 key, std::string value);
    Intent * putExtra(uint32 key, close_callback value);
};
#else
// Allow C code to use `Intent *`
typedef struct Intent Intent;
#endif


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

    enum
    {
        INTENT_EXTRA_GUEST_LIST_FILTER,
        INTENT_EXTRA_RIDE_ID,
        INTENT_EXTRA_PATH,
        INTENT_EXTRA_PEEP,
        INTENT_EXTRA_LOADSAVE_TYPE,
        INTENT_EXTRA_CALLBACK,
        INTENT_EXTRA_TRACK_DESIGN,
        INTENT_EXTRA_RIDE_TYPE,
        INTENT_EXTRA_RIDE_ENTRY_INDEX,
        INTENT_EXTRA_TILE_ELEMENT,
        INTENT_EXTRA_VEHICLE,
        INTENT_EXTRA_MESSAGE,
        INTENT_EXTRA_LIST,
        INTENT_EXTRA_LIST_COUNT,
    };

    enum {
        INTENT_ACTION_MAP,
        INTENT_ACTION_NEW_RIDE_OF_TYPE,
        INTENT_ACTION_REFRESH_NEW_RIDES,
        INTENT_ACTION_REFRESH_RIDE_LIST,
        INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION,
        INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES,
        INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS,
        INTENT_ACTION_INIT_SCENERY,
        INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG,
        INTENT_ACTION_REFRESH_SCENERY,
        INTENT_ACTION_INVALIDATE_TICKER_NEWS,
        INTENT_ACTION_REFRESH_GUEST_LIST,
    };

    Intent *intent_create(rct_windowclass clss);
    void intent_release(Intent * intent);
    void intent_set_string(Intent *, uint32 key, utf8string value);
    void intent_set_pointer(Intent *, uint32 key, void * value);
    void intent_set_sint(Intent *, uint32 key, sint32 value);
    void intent_set_uint(Intent *, uint32 key, uint32 value);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // OPENRCT2_INTENT_H
