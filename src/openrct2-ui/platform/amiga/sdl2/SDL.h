/*****************************************************************************
 * AmigaOS SDL2 shim for openrct2-ui.
 *
 * openrct2-ui talks to SDL2 only for the window, the event pump, keyboard and
 * mouse state, cursors, the clipboard, timers and message boxes (the audio
 * and OpenGL parts are compiled out on AmigaOS). This header declares exactly
 * that subset with SDL2's names, values and semantics, implemented in
 * ../sdl_shim.cpp on Intuition + CyberGraphX. Every other SDL_*.h in this
 * directory just includes this one.
 *****************************************************************************/
#pragma once

#include <cstdint>

typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;
typedef int32_t Sint32;
typedef int16_t Sint16;

typedef enum
{
    SDL_FALSE = 0,
    SDL_TRUE = 1
} SDL_bool;

#define SDL_INIT_AUDIO 0x00000010u
#define SDL_INIT_VIDEO 0x00000020u
#define SDL_INIT_JOYSTICK 0x00000200u
#define SDL_INIT_GAMECONTROLLER 0x00002000u

#define SDL_ENABLE 1
#define SDL_DISABLE 0
#define SDL_QUERY -1

// ---- scancodes (USB HID usage ids, as SDL2) ----
typedef enum
{
    SDL_SCANCODE_UNKNOWN = 0,
    SDL_SCANCODE_A = 4,
    SDL_SCANCODE_B = 5,
    SDL_SCANCODE_C = 6,
    SDL_SCANCODE_D = 7,
    SDL_SCANCODE_E = 8,
    SDL_SCANCODE_F = 9,
    SDL_SCANCODE_G = 10,
    SDL_SCANCODE_H = 11,
    SDL_SCANCODE_I = 12,
    SDL_SCANCODE_J = 13,
    SDL_SCANCODE_K = 14,
    SDL_SCANCODE_L = 15,
    SDL_SCANCODE_M = 16,
    SDL_SCANCODE_N = 17,
    SDL_SCANCODE_O = 18,
    SDL_SCANCODE_P = 19,
    SDL_SCANCODE_Q = 20,
    SDL_SCANCODE_R = 21,
    SDL_SCANCODE_S = 22,
    SDL_SCANCODE_T = 23,
    SDL_SCANCODE_U = 24,
    SDL_SCANCODE_V = 25,
    SDL_SCANCODE_W = 26,
    SDL_SCANCODE_X = 27,
    SDL_SCANCODE_Y = 28,
    SDL_SCANCODE_Z = 29,
    SDL_SCANCODE_1 = 30,
    SDL_SCANCODE_2 = 31,
    SDL_SCANCODE_3 = 32,
    SDL_SCANCODE_4 = 33,
    SDL_SCANCODE_5 = 34,
    SDL_SCANCODE_6 = 35,
    SDL_SCANCODE_7 = 36,
    SDL_SCANCODE_8 = 37,
    SDL_SCANCODE_9 = 38,
    SDL_SCANCODE_0 = 39,
    SDL_SCANCODE_RETURN = 40,
    SDL_SCANCODE_ESCAPE = 41,
    SDL_SCANCODE_BACKSPACE = 42,
    SDL_SCANCODE_TAB = 43,
    SDL_SCANCODE_SPACE = 44,
    SDL_SCANCODE_MINUS = 45,
    SDL_SCANCODE_EQUALS = 46,
    SDL_SCANCODE_LEFTBRACKET = 47,
    SDL_SCANCODE_RIGHTBRACKET = 48,
    SDL_SCANCODE_BACKSLASH = 49,
    SDL_SCANCODE_NONUSHASH = 50,
    SDL_SCANCODE_SEMICOLON = 51,
    SDL_SCANCODE_APOSTROPHE = 52,
    SDL_SCANCODE_GRAVE = 53,
    SDL_SCANCODE_COMMA = 54,
    SDL_SCANCODE_PERIOD = 55,
    SDL_SCANCODE_SLASH = 56,
    SDL_SCANCODE_CAPSLOCK = 57,
    SDL_SCANCODE_F1 = 58,
    SDL_SCANCODE_F2 = 59,
    SDL_SCANCODE_F3 = 60,
    SDL_SCANCODE_F4 = 61,
    SDL_SCANCODE_F5 = 62,
    SDL_SCANCODE_F6 = 63,
    SDL_SCANCODE_F7 = 64,
    SDL_SCANCODE_F8 = 65,
    SDL_SCANCODE_F9 = 66,
    SDL_SCANCODE_F10 = 67,
    SDL_SCANCODE_F11 = 68,
    SDL_SCANCODE_F12 = 69,
    SDL_SCANCODE_PRINTSCREEN = 70,
    SDL_SCANCODE_SCROLLLOCK = 71,
    SDL_SCANCODE_PAUSE = 72,
    SDL_SCANCODE_INSERT = 73,
    SDL_SCANCODE_HOME = 74,
    SDL_SCANCODE_PAGEUP = 75,
    SDL_SCANCODE_DELETE = 76,
    SDL_SCANCODE_END = 77,
    SDL_SCANCODE_PAGEDOWN = 78,
    SDL_SCANCODE_RIGHT = 79,
    SDL_SCANCODE_LEFT = 80,
    SDL_SCANCODE_DOWN = 81,
    SDL_SCANCODE_UP = 82,
    SDL_SCANCODE_NUMLOCKCLEAR = 83,
    SDL_SCANCODE_KP_DIVIDE = 84,
    SDL_SCANCODE_KP_MULTIPLY = 85,
    SDL_SCANCODE_KP_MINUS = 86,
    SDL_SCANCODE_KP_PLUS = 87,
    SDL_SCANCODE_KP_ENTER = 88,
    SDL_SCANCODE_KP_1 = 89,
    SDL_SCANCODE_KP_2 = 90,
    SDL_SCANCODE_KP_3 = 91,
    SDL_SCANCODE_KP_4 = 92,
    SDL_SCANCODE_KP_5 = 93,
    SDL_SCANCODE_KP_6 = 94,
    SDL_SCANCODE_KP_7 = 95,
    SDL_SCANCODE_KP_8 = 96,
    SDL_SCANCODE_KP_9 = 97,
    SDL_SCANCODE_KP_0 = 98,
    SDL_SCANCODE_KP_PERIOD = 99,
    SDL_SCANCODE_NONUSBACKSLASH = 100,
    SDL_SCANCODE_APPLICATION = 101,
    SDL_SCANCODE_KP_EQUALS = 103,
    SDL_SCANCODE_EXECUTE = 116,
    SDL_SCANCODE_HELP = 117,
    SDL_SCANCODE_MENU = 118,
    SDL_SCANCODE_SELECT = 119,
    SDL_SCANCODE_SYSREQ = 154,
    SDL_SCANCODE_CLEAR = 156,
    SDL_SCANCODE_LCTRL = 224,
    SDL_SCANCODE_LSHIFT = 225,
    SDL_SCANCODE_LALT = 226,
    SDL_SCANCODE_LGUI = 227,
    SDL_SCANCODE_RCTRL = 228,
    SDL_SCANCODE_RSHIFT = 229,
    SDL_SCANCODE_RALT = 230,
    SDL_SCANCODE_RGUI = 231,
    SDL_NUM_SCANCODES = 512
} SDL_Scancode;

// ---- keycodes ----
typedef Sint32 SDL_Keycode;
#define SDLK_SCANCODE_MASK (1 << 30)
#define SDL_SCANCODE_TO_KEYCODE(X) (X | SDLK_SCANCODE_MASK)

enum
{
    SDLK_UNKNOWN = 0,
    SDLK_RETURN = '\r',
    SDLK_ESCAPE = '\x1B',
    SDLK_BACKSPACE = '\b',
    SDLK_TAB = '\t',
    SDLK_SPACE = ' ',
    SDLK_a = 'a',
    SDLK_c = 'c',
    SDLK_v = 'v',
    SDLK_x = 'x',
    SDLK_z = 'z',
    SDLK_CAPSLOCK = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CAPSLOCK),
    SDLK_F1 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F1),
    SDLK_F12 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F12),
    SDLK_PRINTSCREEN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PRINTSCREEN),
    SDLK_SCROLLLOCK = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SCROLLLOCK),
    SDLK_PAUSE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAUSE),
    SDLK_INSERT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_INSERT),
    SDLK_HOME = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HOME),
    SDLK_PAGEUP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEUP),
    SDLK_DELETE = '\x7F',
    SDLK_END = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_END),
    SDLK_PAGEDOWN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_PAGEDOWN),
    SDLK_RIGHT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RIGHT),
    SDLK_LEFT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LEFT),
    SDLK_DOWN = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_DOWN),
    SDLK_UP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_UP),
    SDLK_NUMLOCKCLEAR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_NUMLOCKCLEAR),
    SDLK_KP_DIVIDE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_DIVIDE),
    SDLK_KP_MULTIPLY = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MULTIPLY),
    SDLK_KP_MINUS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_MINUS),
    SDLK_KP_PLUS = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PLUS),
    SDLK_KP_ENTER = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_ENTER),
    SDLK_KP_1 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_1),
    SDLK_KP_2 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_2),
    SDLK_KP_3 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_3),
    SDLK_KP_4 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_4),
    SDLK_KP_5 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_5),
    SDLK_KP_6 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_6),
    SDLK_KP_7 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_7),
    SDLK_KP_8 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_8),
    SDLK_KP_9 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_9),
    SDLK_KP_0 = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_0),
    SDLK_KP_PERIOD = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_KP_PERIOD),
    SDLK_APPLICATION = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_APPLICATION),
    SDLK_EXECUTE = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_EXECUTE),
    SDLK_HELP = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_HELP),
    SDLK_MENU = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_MENU),
    SDLK_SELECT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SELECT),
    SDLK_SYSREQ = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_SYSREQ),
    SDLK_CLEAR = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_CLEAR),
    SDLK_LCTRL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LCTRL),
    SDLK_LSHIFT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LSHIFT),
    SDLK_LALT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LALT),
    SDLK_LGUI = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_LGUI),
    SDLK_RCTRL = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RCTRL),
    SDLK_RSHIFT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RSHIFT),
    SDLK_RALT = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RALT),
    SDLK_RGUI = SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_RGUI),
};

typedef enum
{
    KMOD_NONE = 0x0000,
    KMOD_LSHIFT = 0x0001,
    KMOD_RSHIFT = 0x0002,
    KMOD_LCTRL = 0x0040,
    KMOD_RCTRL = 0x0080,
    KMOD_LALT = 0x0100,
    KMOD_RALT = 0x0200,
    KMOD_LGUI = 0x0400,
    KMOD_RGUI = 0x0800,
    KMOD_NUM = 0x1000,
    KMOD_CAPS = 0x2000,
    KMOD_MODE = 0x4000,
    KMOD_SCROLL = 0x8000,
    KMOD_CTRL = KMOD_LCTRL | KMOD_RCTRL,
    KMOD_SHIFT = KMOD_LSHIFT | KMOD_RSHIFT,
    KMOD_ALT = KMOD_LALT | KMOD_RALT,
    KMOD_GUI = KMOD_LGUI | KMOD_RGUI,
} SDL_Keymod;

typedef struct SDL_Keysym
{
    SDL_Scancode scancode;
    SDL_Keycode sym;
    Uint16 mod;
    Uint32 unused;
} SDL_Keysym;

// ---- events ----
typedef enum
{
    SDL_FIRSTEVENT = 0,
    SDL_QUIT = 0x100,
    SDL_WINDOWEVENT = 0x200,
    SDL_KEYDOWN = 0x300,
    SDL_KEYUP = 0x301,
    SDL_TEXTEDITING = 0x302,
    SDL_TEXTINPUT = 0x303,
    SDL_MOUSEMOTION = 0x400,
    SDL_MOUSEBUTTONDOWN = 0x401,
    SDL_MOUSEBUTTONUP = 0x402,
    SDL_MOUSEWHEEL = 0x403,
    SDL_JOYAXISMOTION = 0x600,
    SDL_JOYHATMOTION = 0x602,
    SDL_JOYBUTTONDOWN = 0x603,
    SDL_JOYBUTTONUP = 0x604,
    SDL_JOYDEVICEADDED = 0x605,
    SDL_JOYDEVICEREMOVED = 0x606,
    SDL_CONTROLLERAXISMOTION = 0x650,
    SDL_CONTROLLERBUTTONDOWN = 0x651,
    SDL_CONTROLLERBUTTONUP = 0x652,
    SDL_CONTROLLERDEVICEADDED = 0x653,
    SDL_CONTROLLERDEVICEREMOVED = 0x654,
    SDL_FINGERDOWN = 0x700,
    SDL_FINGERUP = 0x701,
    SDL_FINGERMOTION = 0x702,
    SDL_MULTIGESTURE = 0x802,
    SDL_LASTEVENT = 0xFFFF
} SDL_EventType;

typedef enum
{
    SDL_WINDOWEVENT_NONE,
    SDL_WINDOWEVENT_SHOWN,
    SDL_WINDOWEVENT_HIDDEN,
    SDL_WINDOWEVENT_EXPOSED,
    SDL_WINDOWEVENT_MOVED,
    SDL_WINDOWEVENT_RESIZED,
    SDL_WINDOWEVENT_SIZE_CHANGED,
    SDL_WINDOWEVENT_MINIMIZED,
    SDL_WINDOWEVENT_MAXIMIZED,
    SDL_WINDOWEVENT_RESTORED,
    SDL_WINDOWEVENT_ENTER,
    SDL_WINDOWEVENT_LEAVE,
    SDL_WINDOWEVENT_FOCUS_GAINED,
    SDL_WINDOWEVENT_FOCUS_LOST,
    SDL_WINDOWEVENT_CLOSE
} SDL_WindowEventID;

#define SDL_TEXTEDITINGEVENT_TEXT_SIZE 32
#define SDL_TEXTINPUTEVENT_TEXT_SIZE 32

typedef struct SDL_WindowEvent
{
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint8 event;
    Sint32 data1;
    Sint32 data2;
} SDL_WindowEvent;

typedef struct SDL_KeyboardEvent
{
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint8 state;
    Uint8 repeat;
    SDL_Keysym keysym;
} SDL_KeyboardEvent;

typedef struct SDL_TextEditingEvent
{
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    char text[SDL_TEXTEDITINGEVENT_TEXT_SIZE];
    Sint32 start;
    Sint32 length;
} SDL_TextEditingEvent;

typedef struct SDL_TextInputEvent
{
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    char text[SDL_TEXTINPUTEVENT_TEXT_SIZE];
} SDL_TextInputEvent;

typedef struct SDL_MouseMotionEvent
{
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint32 which;
    Uint32 state;
    Sint32 x;
    Sint32 y;
    Sint32 xrel;
    Sint32 yrel;
} SDL_MouseMotionEvent;

typedef struct SDL_MouseButtonEvent
{
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint32 which;
    Uint8 button;
    Uint8 state;
    Uint8 clicks;
    Sint32 x;
    Sint32 y;
} SDL_MouseButtonEvent;

typedef struct SDL_MouseWheelEvent
{
    Uint32 type;
    Uint32 timestamp;
    Uint32 windowID;
    Uint32 which;
    Sint32 x;
    Sint32 y;
} SDL_MouseWheelEvent;

typedef struct SDL_JoyHatEvent
{
    Uint32 type;
    Uint32 timestamp;
    Sint32 which;
    Uint8 hat;
    Uint8 value;
} SDL_JoyHatEvent;

typedef struct SDL_JoyButtonEvent
{
    Uint32 type;
    Uint32 timestamp;
    Sint32 which;
    Uint8 button;
    Uint8 state;
} SDL_JoyButtonEvent;

typedef struct SDL_ControllerAxisEvent
{
    Uint32 type;
    Uint32 timestamp;
    Sint32 which;
    Uint8 axis;
    Sint16 value;
} SDL_ControllerAxisEvent;

typedef struct SDL_ControllerButtonEvent
{
    Uint32 type;
    Uint32 timestamp;
    Sint32 which;
    Uint8 button;
    Uint8 state;
} SDL_ControllerButtonEvent;

typedef struct SDL_TouchFingerEvent
{
    Uint32 type;
    Uint32 timestamp;
    float x;
    float y;
} SDL_TouchFingerEvent;

typedef struct SDL_MultiGestureEvent
{
    Uint32 type;
    Uint32 timestamp;
    float dTheta;
    float dDist;
    float x;
    float y;
    Uint16 numFingers;
} SDL_MultiGestureEvent;

typedef union SDL_Event
{
    Uint32 type;
    SDL_WindowEvent window;
    SDL_KeyboardEvent key;
    SDL_TextEditingEvent edit;
    SDL_TextInputEvent text;
    SDL_MouseMotionEvent motion;
    SDL_MouseButtonEvent button;
    SDL_MouseWheelEvent wheel;
    SDL_JoyHatEvent jhat;
    SDL_JoyButtonEvent jbutton;
    SDL_ControllerAxisEvent caxis;
    SDL_ControllerButtonEvent cbutton;
    SDL_TouchFingerEvent tfinger;
    SDL_MultiGestureEvent mgesture;
    Uint8 padding[64];
} SDL_Event;

#define SDL_BUTTON_LEFT 1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT 3
#define SDL_BUTTON_X1 4
#define SDL_BUTTON_X2 5
#define SDL_BUTTON(X) (1u << ((X) - 1))
#define SDL_BUTTON_LMASK SDL_BUTTON(SDL_BUTTON_LEFT)
#define SDL_BUTTON_MMASK SDL_BUTTON(SDL_BUTTON_MIDDLE)
#define SDL_BUTTON_RMASK SDL_BUTTON(SDL_BUTTON_RIGHT)
#define SDL_TOUCH_MOUSEID ((Uint32)-1)

#define SDL_HAT_CENTERED 0x00
#define SDL_HAT_UP 0x01
#define SDL_HAT_RIGHT 0x02
#define SDL_HAT_DOWN 0x04
#define SDL_HAT_LEFT 0x08

typedef enum
{
    SDL_CONTROLLER_AXIS_INVALID = -1,
    SDL_CONTROLLER_AXIS_LEFTX,
    SDL_CONTROLLER_AXIS_LEFTY,
    SDL_CONTROLLER_AXIS_RIGHTX,
    SDL_CONTROLLER_AXIS_RIGHTY,
    SDL_CONTROLLER_AXIS_TRIGGERLEFT,
    SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
    SDL_CONTROLLER_AXIS_MAX
} SDL_GameControllerAxis;

// ---- window ----
struct SDL_Window; // defined in ../AmigaWindow.h
typedef struct SDL_Window SDL_Window;
struct SDL_Renderer;
typedef struct SDL_Renderer SDL_Renderer;

typedef enum
{
    SDL_WINDOW_FULLSCREEN = 0x00000001,
    SDL_WINDOW_OPENGL = 0x00000002,
    SDL_WINDOW_SHOWN = 0x00000004,
    SDL_WINDOW_HIDDEN = 0x00000008,
    SDL_WINDOW_BORDERLESS = 0x00000010,
    SDL_WINDOW_RESIZABLE = 0x00000020,
    SDL_WINDOW_MINIMIZED = 0x00000040,
    SDL_WINDOW_MAXIMIZED = 0x00000080,
    SDL_WINDOW_INPUT_GRABBED = 0x00000100,
    SDL_WINDOW_INPUT_FOCUS = 0x00000200,
    SDL_WINDOW_MOUSE_FOCUS = 0x00000400,
    SDL_WINDOW_FULLSCREEN_DESKTOP = (SDL_WINDOW_FULLSCREEN | 0x00001000),
    SDL_WINDOW_ALLOW_HIGHDPI = 0x00002000,
} SDL_WindowFlags;

#define SDL_WINDOWPOS_UNDEFINED_MASK 0x1FFF0000u
#define SDL_WINDOWPOS_UNDEFINED_DISPLAY(X) (SDL_WINDOWPOS_UNDEFINED_MASK | (X))
#define SDL_WINDOWPOS_UNDEFINED SDL_WINDOWPOS_UNDEFINED_DISPLAY(0)

typedef enum
{
    SDL_FLASH_CANCEL,
    SDL_FLASH_BRIEFLY,
    SDL_FLASH_UNTIL_FOCUSED
} SDL_FlashOperation;

typedef struct SDL_DisplayMode
{
    Uint32 format;
    int w;
    int h;
    int refresh_rate;
    void* driverdata;
} SDL_DisplayMode;

typedef struct SDL_Rect
{
    int x, y, w, h;
} SDL_Rect;

typedef struct SDL_version
{
    Uint8 major;
    Uint8 minor;
    Uint8 patch;
} SDL_version;

#define SDL_VERSION_ATLEAST(X, Y, Z) 0

// ---- cursors ----
struct SDL_Cursor;
typedef struct SDL_Cursor SDL_Cursor;
typedef enum
{
    SDL_SYSTEM_CURSOR_ARROW,
    SDL_SYSTEM_CURSOR_IBEAM,
    SDL_SYSTEM_CURSOR_WAIT,
    SDL_SYSTEM_CURSOR_CROSSHAIR,
    SDL_SYSTEM_CURSOR_WAITARROW,
    SDL_SYSTEM_CURSOR_SIZENWSE,
    SDL_SYSTEM_CURSOR_SIZENESW,
    SDL_SYSTEM_CURSOR_SIZEWE,
    SDL_SYSTEM_CURSOR_SIZENS,
    SDL_SYSTEM_CURSOR_SIZEALL,
    SDL_SYSTEM_CURSOR_NO,
    SDL_SYSTEM_CURSOR_HAND,
    SDL_NUM_SYSTEM_CURSORS
} SDL_SystemCursor;

// ---- message box ----
typedef struct SDL_MessageBoxButtonData
{
    Uint32 flags;
    int buttonid;
    const char* text;
} SDL_MessageBoxButtonData;

typedef struct SDL_MessageBoxData
{
    Uint32 flags;
    SDL_Window* window;
    const char* title;
    const char* message;
    int numbuttons;
    const SDL_MessageBoxButtonData* buttons;
    const void* colorScheme;
} SDL_MessageBoxData;

// ---- hints ----
#define SDL_HINT_RENDER_SCALE_QUALITY "SDL_RENDER_SCALE_QUALITY"
#define SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS "SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS"

// ---- joystick / game controller (always empty on AmigaOS) ----
struct _SDL_GameController;
typedef struct _SDL_GameController SDL_GameController;
struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;


/* ---- audio + rwops (backed by ahi.device, see sdl_shim_audio.cpp) ---- */
typedef int64_t Sint64;
typedef uint64_t Uint64;
typedef Uint16 SDL_AudioFormat;
#define AUDIO_U8 0x0008
#define AUDIO_S8 0x8008
#define AUDIO_U16LSB 0x0010
#define AUDIO_S16LSB 0x8010
#define AUDIO_U16MSB 0x1010
#define AUDIO_S16MSB 0x9010
#define AUDIO_S16 AUDIO_S16LSB
/* 68k is big-endian: the native 16-bit format is MSB first. */
#define AUDIO_S16SYS AUDIO_S16MSB
#define SDL_AUDIO_MASK_BITSIZE 0xFF
#define SDL_AUDIO_BITSIZE(x) ((x) & SDL_AUDIO_MASK_BITSIZE)
#define SDL_AUDIO_ISBIGENDIAN(x) ((x) & 0x1000)
#define SDL_AUDIO_ISSIGNED(x) ((x) & 0x8000)
#define SDL_MIX_MAXVOLUME 128
#define SDL_AUDIO_ALLOW_FREQUENCY_CHANGE 0x00000001
#define SDL_AUDIO_ALLOW_FORMAT_CHANGE 0x00000002
#define SDL_AUDIO_ALLOW_CHANNELS_CHANGE 0x00000004
#define SDL_AUDIO_ALLOW_SAMPLES_CHANGE 0x00000008
typedef Uint32 SDL_AudioDeviceID;
typedef void (*SDL_AudioCallback)(void* userdata, Uint8* stream, int len);
typedef struct SDL_AudioSpec
{
    int freq;
    SDL_AudioFormat format;
    Uint8 channels;
    Uint8 silence;
    Uint16 samples;
    Uint16 padding;
    Uint32 size;
    SDL_AudioCallback callback;
    void* userdata;
} SDL_AudioSpec;
typedef struct SDL_AudioCVT
{
    int needed;
    SDL_AudioFormat src_format;
    SDL_AudioFormat dst_format;
    double rate_incr;
    Uint8* buf;
    int len;
    int len_cvt;
    int len_mult;
    double len_ratio;
    /* shim-private: the full source/destination description */
    int src_channels;
    int dst_channels;
    int src_rate;
    int dst_rate;
} SDL_AudioCVT;

#define RW_SEEK_SET 0
#define RW_SEEK_CUR 1
#define RW_SEEK_END 2
#define SDL_RWOPS_UNKNOWN 0U
typedef struct SDL_RWops
{
    Sint64 (*size)(struct SDL_RWops* context);
    Sint64 (*seek)(struct SDL_RWops* context, Sint64 offset, int whence);
    size_t (*read)(struct SDL_RWops* context, void* ptr, size_t size, size_t maxnum);
    size_t (*write)(struct SDL_RWops* context, const void* ptr, size_t size, size_t num);
    int (*close)(struct SDL_RWops* context);
    Uint32 type;
    union
    {
        struct
        {
            void* data1;
            void* data2;
        } unknown;
    } hidden;
} SDL_RWops;

extern "C" {
int SDL_Init(Uint32 flags);
void SDL_Quit(void);
void SDL_QuitSubSystem(Uint32 flags);
const char* SDL_GetError(void);
void SDL_GetVersion(SDL_version* ver);
void SDL_free(void* mem);
Uint32 SDL_GetTicks(void);
SDL_bool SDL_SetHint(const char* name, const char* value);

SDL_Window* SDL_CreateWindow(const char* title, int x, int y, int w, int h, Uint32 flags);
void SDL_DestroyWindow(SDL_Window* window);
Uint32 SDL_GetWindowFlags(SDL_Window* window);
void SDL_GetWindowSize(SDL_Window* window, int* w, int* h);
void SDL_SetWindowSize(SDL_Window* window, int w, int h);
void SDL_SetWindowMinimumSize(SDL_Window* window, int min_w, int min_h);
void SDL_GetWindowPosition(SDL_Window* window, int* x, int* y);
int SDL_SetWindowFullscreen(SDL_Window* window, Uint32 flags);
int SDL_GetWindowDisplayIndex(SDL_Window* window);
void SDL_SetWindowGrab(SDL_Window* window, SDL_bool grabbed);
int SDL_FlashWindow(SDL_Window* window, SDL_FlashOperation operation);
SDL_Renderer* SDL_GetRenderer(SDL_Window* window);
int SDL_GetRendererOutputSize(SDL_Renderer* renderer, int* w, int* h);
int SDL_GetNumDisplayModes(int displayIndex);
int SDL_GetDisplayMode(int displayIndex, int modeIndex, SDL_DisplayMode* mode);
int SDL_GetDesktopDisplayMode(int displayIndex, SDL_DisplayMode* mode);
void SDL_DisableScreenSaver(void);
void SDL_EnableScreenSaver(void);

int SDL_PollEvent(SDL_Event* event);
SDL_Keymod SDL_GetModState(void);
const Uint8* SDL_GetKeyboardState(int* numkeys);
Uint32 SDL_GetMouseState(int* x, int* y);
void SDL_WarpMouseInWindow(SDL_Window* window, int x, int y);
int SDL_ShowCursor(int toggle);
SDL_Keycode SDL_GetKeyFromScancode(SDL_Scancode scancode);
SDL_Scancode SDL_GetScancodeFromKey(SDL_Keycode key);
const char* SDL_GetKeyName(SDL_Keycode key);
SDL_Keycode SDL_GetKeyFromName(const char* name);
void SDL_StartTextInput(void);
void SDL_StopTextInput(void);
SDL_bool SDL_IsTextInputActive(void);
void SDL_SetTextInputRect(const SDL_Rect* rect);

SDL_Cursor* SDL_CreateCursor(const Uint8* data, const Uint8* mask, int w, int h, int hot_x, int hot_y);
SDL_Cursor* SDL_CreateSystemCursor(SDL_SystemCursor id);
void SDL_SetCursor(SDL_Cursor* cursor);
void SDL_FreeCursor(SDL_Cursor* cursor);

int SDL_SetClipboardText(const char* text);
char* SDL_GetClipboardText(void);
SDL_bool SDL_HasClipboardText(void);

int SDL_ShowMessageBox(const SDL_MessageBoxData* messageboxdata, int* buttonid);
int SDL_ShowSimpleMessageBox(Uint32 flags, const char* title, const char* message, SDL_Window* window);

int SDL_NumJoysticks(void);
SDL_bool SDL_IsGameController(int joystick_index);
SDL_GameController* SDL_GameControllerOpen(int joystick_index);
SDL_Joystick* SDL_GameControllerGetJoystick(SDL_GameController* gamecontroller);
Sint16 SDL_GameControllerGetAxis(SDL_GameController* gamecontroller, SDL_GameControllerAxis axis);
Uint8 SDL_JoystickGetButton(SDL_Joystick* joystick, int button);
int SDL_JoystickNumHats(SDL_Joystick* joystick);
Uint8 SDL_JoystickGetHat(SDL_Joystick* joystick, int hat);
/* audio (sdl_shim_audio.cpp) */
SDL_AudioDeviceID SDL_OpenAudioDevice(
    const char* device, int iscapture, const SDL_AudioSpec* desired, SDL_AudioSpec* obtained, int allowed_changes);
void SDL_PauseAudioDevice(SDL_AudioDeviceID dev, int pause_on);
void SDL_LockAudioDevice(SDL_AudioDeviceID dev);
void SDL_UnlockAudioDevice(SDL_AudioDeviceID dev);
void SDL_CloseAudioDevice(SDL_AudioDeviceID dev);
int SDL_GetNumAudioDevices(int iscapture);
const char* SDL_GetAudioDeviceName(int index, int iscapture);
int SDL_BuildAudioCVT(
    SDL_AudioCVT* cvt, SDL_AudioFormat src_format, Uint8 src_channels, int src_rate, SDL_AudioFormat dst_format,
    Uint8 dst_channels, int dst_rate);
int SDL_ConvertAudio(SDL_AudioCVT* cvt);
void SDL_MixAudioFormat(Uint8* dst, const Uint8* src, SDL_AudioFormat format, Uint32 len, int volume);
/* Amiga-only: refill the AHI buffers from the main loop (called by SDL_PollEvent). */
void SDL_amiga_AudioPump(void);
/* rwops */
void* SDL_malloc(size_t size);
SDL_RWops* SDL_AllocRW(void);
void SDL_FreeRW(SDL_RWops* area);
Sint64 SDL_RWsize(SDL_RWops* context);
Sint64 SDL_RWseek(SDL_RWops* context, Sint64 offset, int whence);
Sint64 SDL_RWtell(SDL_RWops* context);
size_t SDL_RWread(SDL_RWops* context, void* ptr, size_t size, size_t maxnum);
size_t SDL_RWwrite(SDL_RWops* context, const void* ptr, size_t size, size_t num);
int SDL_RWclose(SDL_RWops* context);
Uint16 SDL_ReadLE16(SDL_RWops* src);
Uint32 SDL_ReadLE32(SDL_RWops* src);
}

