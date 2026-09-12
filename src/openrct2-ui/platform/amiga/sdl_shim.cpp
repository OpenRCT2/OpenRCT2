/*****************************************************************************
 * SDL2 shim for openrct2-ui on AmigaOS: the SDL2 subset the UI uses,
 * implemented on top of the plain-C Intuition/CyberGraphX layer (amiga_ui.c).
 *****************************************************************************/
#ifdef __amigaos__

    #include "AmigaWindow.h"
    #include "amiga_ui.h"
#include "amiga_audio.h"
    #include "sdl2/SDL.h"

    #include <cctype>
    #include <cstdio>
    #include <cstdlib>
    #include <cstring>
    #include <deque>
    #include <string>

extern "C" unsigned amiga_ticks_ms(void);
extern "C" void amiga_trace(const char* line);

namespace
{
    SDL_Window* g_window = nullptr;
    std::deque<SDL_Event> g_queue;
    Uint8 g_keyState[SDL_NUM_SCANCODES] = {};
    Uint16 g_modState = 0;
    bool g_textInput = false;
    char g_error[256] = "";
    bool g_pointerVisible = true;

    // Amiga raw key code -> SDL scancode (US layout for the positional keys; the keymap supplies the characters)
    const SDL_Scancode kRawToScancode[128] = {
        /* 00 */ SDL_SCANCODE_GRAVE, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4, SDL_SCANCODE_5,
        SDL_SCANCODE_6, SDL_SCANCODE_7,
        /* 08 */ SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_0, SDL_SCANCODE_MINUS, SDL_SCANCODE_EQUALS,
        SDL_SCANCODE_BACKSLASH, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_KP_0,
        /* 10 */ SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R, SDL_SCANCODE_T, SDL_SCANCODE_Y,
        SDL_SCANCODE_U, SDL_SCANCODE_I,
        /* 18 */ SDL_SCANCODE_O, SDL_SCANCODE_P, SDL_SCANCODE_LEFTBRACKET, SDL_SCANCODE_RIGHTBRACKET, SDL_SCANCODE_UNKNOWN,
        SDL_SCANCODE_KP_1, SDL_SCANCODE_KP_2, SDL_SCANCODE_KP_3,
        /* 20 */ SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F, SDL_SCANCODE_G, SDL_SCANCODE_H,
        SDL_SCANCODE_J, SDL_SCANCODE_K,
        /* 28 */ SDL_SCANCODE_L, SDL_SCANCODE_SEMICOLON, SDL_SCANCODE_APOSTROPHE, SDL_SCANCODE_NONUSHASH,
        SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_KP_4, SDL_SCANCODE_KP_5, SDL_SCANCODE_KP_6,
        /* 30 */ SDL_SCANCODE_NONUSBACKSLASH, SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V,
        SDL_SCANCODE_B, SDL_SCANCODE_N, SDL_SCANCODE_M,
        /* 38 */ SDL_SCANCODE_COMMA, SDL_SCANCODE_PERIOD, SDL_SCANCODE_SLASH, SDL_SCANCODE_UNKNOWN,
        SDL_SCANCODE_KP_PERIOD, SDL_SCANCODE_KP_7, SDL_SCANCODE_KP_8, SDL_SCANCODE_KP_9,
        /* 40 */ SDL_SCANCODE_SPACE, SDL_SCANCODE_BACKSPACE, SDL_SCANCODE_TAB, SDL_SCANCODE_KP_ENTER, SDL_SCANCODE_RETURN,
        SDL_SCANCODE_ESCAPE, SDL_SCANCODE_DELETE, SDL_SCANCODE_INSERT,
        /* 48 */ SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN, SDL_SCANCODE_KP_MINUS, SDL_SCANCODE_F11, SDL_SCANCODE_UP,
        SDL_SCANCODE_DOWN, SDL_SCANCODE_RIGHT, SDL_SCANCODE_LEFT,
        /* 50 */ SDL_SCANCODE_F1, SDL_SCANCODE_F2, SDL_SCANCODE_F3, SDL_SCANCODE_F4, SDL_SCANCODE_F5, SDL_SCANCODE_F6,
        SDL_SCANCODE_F7, SDL_SCANCODE_F8,
        /* 58 */ SDL_SCANCODE_F9, SDL_SCANCODE_F10, SDL_SCANCODE_NUMLOCKCLEAR, SDL_SCANCODE_SCROLLLOCK,
        SDL_SCANCODE_KP_DIVIDE, SDL_SCANCODE_KP_MULTIPLY, SDL_SCANCODE_KP_PLUS, SDL_SCANCODE_HELP,
        /* 60 */ SDL_SCANCODE_LSHIFT, SDL_SCANCODE_RSHIFT, SDL_SCANCODE_CAPSLOCK, SDL_SCANCODE_LCTRL, SDL_SCANCODE_LALT,
        SDL_SCANCODE_RALT, SDL_SCANCODE_LGUI, SDL_SCANCODE_RGUI,
        /* 68 */ SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_MENU, SDL_SCANCODE_UNKNOWN,
        SDL_SCANCODE_PRINTSCREEN, SDL_SCANCODE_PAUSE, SDL_SCANCODE_F12,
        /* 70 */ SDL_SCANCODE_HOME, SDL_SCANCODE_END, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
        SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
        /* 78 */ SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
        SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN,
    };

    // IEQUALIFIER bits (devices/inputevent.h)
    constexpr int kIEQ_LSHIFT = 0x0001, kIEQ_RSHIFT = 0x0002, kIEQ_CAPSLOCK = 0x0004, kIEQ_CONTROL = 0x0008,
                  kIEQ_LALT = 0x0010, kIEQ_RALT = 0x0020, kIEQ_LCOMMAND = 0x0040, kIEQ_RCOMMAND = 0x0080,
                  kIEQ_NUMERICPAD = 0x0100, kIEQ_REPEAT = 0x0200;

    Uint16 QualifierToMod(int q)
    {
        Uint16 m = 0;
        if (q & kIEQ_LSHIFT)
            m |= KMOD_LSHIFT;
        if (q & kIEQ_RSHIFT)
            m |= KMOD_RSHIFT;
        if (q & kIEQ_CONTROL)
            m |= KMOD_LCTRL;
        if (q & kIEQ_LALT)
            m |= KMOD_LALT;
        if (q & kIEQ_RALT)
            m |= KMOD_RALT;
        if (q & kIEQ_LCOMMAND)
            m |= KMOD_LGUI;
        if (q & kIEQ_RCOMMAND)
            m |= KMOD_RGUI;
        if (q & kIEQ_CAPSLOCK)
            m |= KMOD_CAPS;
        // The Amiga has no num lock; treat the keypad as numeric (SDL's KMOD_NUM set) so KP digits type digits.
        m |= KMOD_NUM;
        return m;
    }

    // Character (Latin-1) for a raw key with the shift/alt/ctrl qualifiers stripped: SDL keycodes are the unshifted key.
    SDL_Keycode KeycodeForRaw(int code, int qual, SDL_Scancode sc)
    {
        char buf[4];
        int n = amiga_ui_map_key(code & 0x7F, qual & kIEQ_NUMERICPAD, buf, sizeof buf);
        if (n == 1)
        {
            unsigned char c = static_cast<unsigned char>(buf[0]);
            if (c >= 0x20 && c != 0x7F)
                return static_cast<SDL_Keycode>(std::tolower(c));
        }
        switch (sc)
        {
            case SDL_SCANCODE_RETURN:
                return SDLK_RETURN;
            case SDL_SCANCODE_ESCAPE:
                return SDLK_ESCAPE;
            case SDL_SCANCODE_BACKSPACE:
                return SDLK_BACKSPACE;
            case SDL_SCANCODE_TAB:
                return SDLK_TAB;
            case SDL_SCANCODE_SPACE:
                return SDLK_SPACE;
            case SDL_SCANCODE_DELETE:
                return SDLK_DELETE;
            case SDL_SCANCODE_UNKNOWN:
                return SDLK_UNKNOWN;
            default:
                return SDL_SCANCODE_TO_KEYCODE(sc);
        }
    }

    void Push(const SDL_Event& e)
    {
        g_queue.push_back(e);
    }

    void PumpOS()
    {
        amiga_ui_event ev;
        while (amiga_ui_poll(&ev))
        {
            SDL_Event e{};
            e.type = 0;
            switch (ev.type)
            {
                case AMIGA_UI_EV_MOUSEMOVE:
                    e.type = SDL_MOUSEMOTION;
                    e.motion.timestamp = amiga_ticks_ms();
                    e.motion.x = ev.x;
                    e.motion.y = ev.y;
                    Push(e);
                    break;
                case AMIGA_UI_EV_BUTTON:
                    e.type = (ev.qual & 0x8000) ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
                    e.button.timestamp = amiga_ticks_ms();
                    e.button.which = 0;
                    e.button.button = static_cast<Uint8>(ev.code);
                    e.button.state = (ev.qual & 0x8000) ? 1 : 0;
                    e.button.clicks = 1;
                    e.button.x = ev.x;
                    e.button.y = ev.y;
                    Push(e);
                    break;
                case AMIGA_UI_EV_WHEEL:
                    e.type = SDL_MOUSEWHEEL;
                    e.wheel.timestamp = amiga_ticks_ms();
                    e.wheel.y = ev.code;
                    Push(e);
                    break;
                case AMIGA_UI_EV_RAWKEY:
                {
                    bool up = (ev.code & 0x80) != 0;
                    int raw = ev.code & 0x7F;
                    SDL_Scancode sc = kRawToScancode[raw];
                    g_modState = QualifierToMod(ev.qual);
                    if (sc == SDL_SCANCODE_UNKNOWN)
                        break;
                    g_keyState[sc] = up ? 0 : 1;
                    e.type = up ? SDL_KEYUP : SDL_KEYDOWN;
                    e.key.timestamp = amiga_ticks_ms();
                    e.key.state = up ? 0 : 1;
                    e.key.repeat = (ev.qual & kIEQ_REPEAT) ? 1 : 0;
                    e.key.keysym.scancode = sc;
                    e.key.keysym.sym = KeycodeForRaw(ev.code, ev.qual, sc);
                    e.key.keysym.mod = g_modState;
                    Push(e);
                    // Text input: the fully qualified character, as UTF-8 (the keymap gives Latin-1)
                    if (!up && g_textInput && ev.ascii >= 0x20 && ev.ascii != 0x7F && !(ev.qual & kIEQ_CONTROL)
                        && !(ev.qual & (kIEQ_LCOMMAND | kIEQ_RCOMMAND)))
                    {
                        SDL_Event t{};
                        t.type = SDL_TEXTINPUT;
                        t.text.timestamp = amiga_ticks_ms();
                        unsigned c = static_cast<unsigned>(ev.ascii);
                        if (c < 0x80)
                        {
                            t.text.text[0] = static_cast<char>(c);
                        }
                        else
                        {
                            t.text.text[0] = static_cast<char>(0xC0 | (c >> 6));
                            t.text.text[1] = static_cast<char>(0x80 | (c & 0x3F));
                        }
                        Push(t);
                    }
                    break;
                }
                case AMIGA_UI_EV_ACTIVE:
                    e.type = SDL_WINDOWEVENT;
                    e.window.timestamp = amiga_ticks_ms();
                    e.window.event = ev.code ? SDL_WINDOWEVENT_FOCUS_GAINED : SDL_WINDOWEVENT_FOCUS_LOST;
                    Push(e);
                    break;
                case AMIGA_UI_EV_CLOSE:
                    e.type = SDL_QUIT;
                    Push(e);
                    break;
                default:
                    break;
            }
        }
    }

    struct KeyName
    {
        SDL_Keycode key;
        const char* name;
    };
    const KeyName kKeyNames[] = {
        { SDLK_RETURN, "Return" },
        { SDLK_ESCAPE, "Escape" },
        { SDLK_BACKSPACE, "Backspace" },
        { SDLK_TAB, "Tab" },
        { SDLK_SPACE, "Space" },
        { SDLK_DELETE, "Delete" },
        { SDLK_CAPSLOCK, "CapsLock" },
        { SDLK_F1, "F1" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F2), "F2" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F3), "F3" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F4), "F4" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F5), "F5" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F6), "F6" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F7), "F7" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F8), "F8" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F9), "F9" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F10), "F10" },
        { SDL_SCANCODE_TO_KEYCODE(SDL_SCANCODE_F11), "F11" },
        { SDLK_F12, "F12" },
        { SDLK_PRINTSCREEN, "PrintScreen" },
        { SDLK_SCROLLLOCK, "ScrollLock" },
        { SDLK_PAUSE, "Pause" },
        { SDLK_INSERT, "Insert" },
        { SDLK_HOME, "Home" },
        { SDLK_PAGEUP, "PageUp" },
        { SDLK_END, "End" },
        { SDLK_PAGEDOWN, "PageDown" },
        { SDLK_RIGHT, "Right" },
        { SDLK_LEFT, "Left" },
        { SDLK_DOWN, "Down" },
        { SDLK_UP, "Up" },
        { SDLK_NUMLOCKCLEAR, "Numlock" },
        { SDLK_KP_DIVIDE, "Keypad /" },
        { SDLK_KP_MULTIPLY, "Keypad *" },
        { SDLK_KP_MINUS, "Keypad -" },
        { SDLK_KP_PLUS, "Keypad +" },
        { SDLK_KP_ENTER, "Keypad Enter" },
        { SDLK_KP_1, "Keypad 1" },
        { SDLK_KP_2, "Keypad 2" },
        { SDLK_KP_3, "Keypad 3" },
        { SDLK_KP_4, "Keypad 4" },
        { SDLK_KP_5, "Keypad 5" },
        { SDLK_KP_6, "Keypad 6" },
        { SDLK_KP_7, "Keypad 7" },
        { SDLK_KP_8, "Keypad 8" },
        { SDLK_KP_9, "Keypad 9" },
        { SDLK_KP_0, "Keypad 0" },
        { SDLK_KP_PERIOD, "Keypad ." },
        { SDLK_HELP, "Help" },
        { SDLK_MENU, "Menu" },
        { SDLK_LCTRL, "Left Ctrl" },
        { SDLK_LSHIFT, "Left Shift" },
        { SDLK_LALT, "Left Alt" },
        { SDLK_LGUI, "Left GUI" },
        { SDLK_RCTRL, "Right Ctrl" },
        { SDLK_RSHIFT, "Right Shift" },
        { SDLK_RALT, "Right Alt" },
        { SDLK_RGUI, "Right GUI" },
    };

    bool IEquals(const char* a, const char* b)
    {
        while (*a && *b)
        {
            if (std::tolower(static_cast<unsigned char>(*a)) != std::tolower(static_cast<unsigned char>(*b)))
                return false;
            a++;
            b++;
        }
        return *a == 0 && *b == 0;
    }
} // namespace

extern "C" {

int SDL_Init(Uint32 flags)
{
    char noAudio[8];
    if ((flags & SDL_INIT_AUDIO) && amiga_audio_getenv("OPENRCT2_NO_AUDIO", noAudio, sizeof noAudio))
    {
        std::snprintf(g_error, sizeof g_error, "audio disabled by OPENRCT2_NO_AUDIO");
        return -1;
    }
    if ((flags & SDL_INIT_AUDIO) && !amiga_audio_available())
    {
        std::snprintf(g_error, sizeof g_error, "ahi.device unit 0 could not be opened (is AHI installed and configured?)");
        return -1;
    }
    return 0;
}
void SDL_Quit(void)
{
    amiga_ui_close();
}
void SDL_QuitSubSystem(Uint32)
{
}
const char* SDL_GetError(void)
{
    return g_error;
}
void SDL_GetVersion(SDL_version* ver)
{
    ver->major = 2;
    ver->minor = 0;
    ver->patch = 0;
}
void SDL_free(void* mem)
{
    std::free(mem);
}
Uint32 SDL_GetTicks(void)
{
    return amiga_ticks_ms();
}
SDL_bool SDL_SetHint(const char*, const char*)
{
    return SDL_TRUE;
}

SDL_Window* SDL_CreateWindow(const char* title, int, int, int w, int h, Uint32)
{
    if (g_window != nullptr)
    {
        std::snprintf(g_error, sizeof g_error, "only one window is supported");
        return nullptr;
    }
    int aw = 0, ah = 0;
    if (!amiga_ui_open(w, h, title, &aw, &ah))
    {
        std::snprintf(g_error, sizeof g_error, "no 8-bit RTG screen mode for %dx%d", w, h);
        return nullptr;
    }
    g_window = new SDL_Window();
    g_window->width = aw;
    g_window->height = ah;
    g_window->title = title;
    g_window->fullscreen = true;
    amiga_trace("sdl: window open");
    return g_window;
}

void SDL_DestroyWindow(SDL_Window* window)
{
    if (window == g_window && g_window != nullptr)
    {
        amiga_ui_close();
        delete g_window;
        g_window = nullptr;
    }
}

Uint32 SDL_GetWindowFlags(SDL_Window* window)
{
    if (window == nullptr)
        return 0;
    return SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_FULLSCREEN;
}

void SDL_GetWindowSize(SDL_Window* window, int* w, int* h)
{
    if (w != nullptr)
        *w = window != nullptr ? window->width : 0;
    if (h != nullptr)
        *h = window != nullptr ? window->height : 0;
}

void SDL_SetWindowSize(SDL_Window* window, int w, int h)
{
    // -1 x -1 means "use the default"; keep the screen we have
    if (window == nullptr || w <= 0 || h <= 0 || (w == window->width && h == window->height))
        return;
    int aw = 0, ah = 0;
    if (amiga_ui_open(w, h, window->title.c_str(), &aw, &ah))
    {
        window->width = aw;
        window->height = ah;
        SDL_Event e{};
        e.type = SDL_WINDOWEVENT;
        e.window.event = SDL_WINDOWEVENT_RESIZED;
        e.window.data1 = aw;
        e.window.data2 = ah;
        Push(e);
    }
}

void SDL_SetWindowMinimumSize(SDL_Window*, int, int)
{
}
void SDL_GetWindowPosition(SDL_Window*, int* x, int* y)
{
    if (x != nullptr)
        *x = 0;
    if (y != nullptr)
        *y = 0;
}
int SDL_SetWindowFullscreen(SDL_Window*, Uint32)
{
    return 0;
}
int SDL_GetWindowDisplayIndex(SDL_Window*)
{
    return 0;
}
void SDL_SetWindowGrab(SDL_Window*, SDL_bool)
{
}
int SDL_FlashWindow(SDL_Window*, SDL_FlashOperation)
{
    return 0;
}
SDL_Renderer* SDL_GetRenderer(SDL_Window*)
{
    return nullptr;
}
int SDL_GetRendererOutputSize(SDL_Renderer*, int*, int*)
{
    return -1;
}

static const int kModes[][2] = { { 640, 480 }, { 800, 600 }, { 1024, 768 }, { 1280, 720 }, { 1280, 1024 }, { 1600, 900 }, { 1920, 1080 } };

int SDL_GetNumDisplayModes(int)
{
    return static_cast<int>(sizeof(kModes) / sizeof(kModes[0]));
}
int SDL_GetDisplayMode(int, int modeIndex, SDL_DisplayMode* mode)
{
    if (modeIndex < 0 || modeIndex >= SDL_GetNumDisplayModes(0))
        return -1;
    *mode = {};
    if (amiga_ui_mode_available(kModes[modeIndex][0], kModes[modeIndex][1]))
    {
        mode->w = kModes[modeIndex][0];
        mode->h = kModes[modeIndex][1];
    }
    return 0;
}
int SDL_GetDesktopDisplayMode(int, SDL_DisplayMode* mode)
{
    *mode = {};
    if (!amiga_ui_desktop_size(&mode->w, &mode->h))
    {
        mode->w = 1280;
        mode->h = 720;
    }
    return 0;
}
void SDL_DisableScreenSaver(void)
{
}
void SDL_EnableScreenSaver(void)
{
}

int SDL_PollEvent(SDL_Event* event)
{
    SDL_amiga_AudioPump(); // refill the AHI buffers from the main loop (no audio thread on AmigaOS)
    PumpOS();
    if (g_queue.empty())
        return 0;
    if (event != nullptr)
        *event = g_queue.front();
    g_queue.pop_front();
    return 1;
}

SDL_Keymod SDL_GetModState(void)
{
    return static_cast<SDL_Keymod>(g_modState);
}
const Uint8* SDL_GetKeyboardState(int* numkeys)
{
    if (numkeys != nullptr)
        *numkeys = SDL_NUM_SCANCODES;
    return g_keyState;
}
Uint32 SDL_GetMouseState(int* x, int* y)
{
    int mask = amiga_ui_mouse(x, y);
    Uint32 r = 0;
    if (mask & 1)
        r |= SDL_BUTTON_LMASK;
    if (mask & 2)
        r |= SDL_BUTTON_MMASK;
    if (mask & 4)
        r |= SDL_BUTTON_RMASK;
    return r;
}
void SDL_WarpMouseInWindow(SDL_Window*, int, int)
{
}
int SDL_ShowCursor(int toggle)
{
    if (toggle == SDL_QUERY)
        return g_pointerVisible ? SDL_ENABLE : SDL_DISABLE;
    bool show = toggle == SDL_ENABLE;
    if (show != g_pointerVisible)
    {
        g_pointerVisible = show;
        amiga_ui_show_pointer(show ? 1 : 0);
    }
    return g_pointerVisible ? SDL_ENABLE : SDL_DISABLE;
}

SDL_Keycode SDL_GetKeyFromScancode(SDL_Scancode scancode)
{
    if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_Z)
        return 'a' + (scancode - SDL_SCANCODE_A);
    if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9)
        return '1' + (scancode - SDL_SCANCODE_1);
    switch (scancode)
    {
        case SDL_SCANCODE_0:
            return '0';
        case SDL_SCANCODE_RETURN:
            return SDLK_RETURN;
        case SDL_SCANCODE_ESCAPE:
            return SDLK_ESCAPE;
        case SDL_SCANCODE_BACKSPACE:
            return SDLK_BACKSPACE;
        case SDL_SCANCODE_TAB:
            return SDLK_TAB;
        case SDL_SCANCODE_SPACE:
            return SDLK_SPACE;
        case SDL_SCANCODE_MINUS:
            return '-';
        case SDL_SCANCODE_EQUALS:
            return '=';
        case SDL_SCANCODE_LEFTBRACKET:
            return '[';
        case SDL_SCANCODE_RIGHTBRACKET:
            return ']';
        case SDL_SCANCODE_BACKSLASH:
            return '\\';
        case SDL_SCANCODE_SEMICOLON:
            return ';';
        case SDL_SCANCODE_APOSTROPHE:
            return '\'';
        case SDL_SCANCODE_GRAVE:
            return '`';
        case SDL_SCANCODE_COMMA:
            return ',';
        case SDL_SCANCODE_PERIOD:
            return '.';
        case SDL_SCANCODE_SLASH:
            return '/';
        case SDL_SCANCODE_DELETE:
            return SDLK_DELETE;
        case SDL_SCANCODE_UNKNOWN:
            return SDLK_UNKNOWN;
        default:
            return SDL_SCANCODE_TO_KEYCODE(scancode);
    }
}

SDL_Scancode SDL_GetScancodeFromKey(SDL_Keycode key)
{
    if (key & SDLK_SCANCODE_MASK)
        return static_cast<SDL_Scancode>(key & ~SDLK_SCANCODE_MASK);
    if (key >= 'a' && key <= 'z')
        return static_cast<SDL_Scancode>(SDL_SCANCODE_A + (key - 'a'));
    if (key >= 'A' && key <= 'Z')
        return static_cast<SDL_Scancode>(SDL_SCANCODE_A + (key - 'A'));
    if (key >= '1' && key <= '9')
        return static_cast<SDL_Scancode>(SDL_SCANCODE_1 + (key - '1'));
    switch (key)
    {
        case '0':
            return SDL_SCANCODE_0;
        case SDLK_RETURN:
            return SDL_SCANCODE_RETURN;
        case SDLK_ESCAPE:
            return SDL_SCANCODE_ESCAPE;
        case SDLK_BACKSPACE:
            return SDL_SCANCODE_BACKSPACE;
        case SDLK_TAB:
            return SDL_SCANCODE_TAB;
        case SDLK_SPACE:
            return SDL_SCANCODE_SPACE;
        case '-':
            return SDL_SCANCODE_MINUS;
        case '=':
            return SDL_SCANCODE_EQUALS;
        case '[':
            return SDL_SCANCODE_LEFTBRACKET;
        case ']':
            return SDL_SCANCODE_RIGHTBRACKET;
        case '\\':
            return SDL_SCANCODE_BACKSLASH;
        case ';':
            return SDL_SCANCODE_SEMICOLON;
        case '\'':
            return SDL_SCANCODE_APOSTROPHE;
        case '`':
            return SDL_SCANCODE_GRAVE;
        case ',':
            return SDL_SCANCODE_COMMA;
        case '.':
            return SDL_SCANCODE_PERIOD;
        case '/':
            return SDL_SCANCODE_SLASH;
        case SDLK_DELETE:
            return SDL_SCANCODE_DELETE;
        default:
            return SDL_SCANCODE_UNKNOWN;
    }
}

const char* SDL_GetKeyName(SDL_Keycode key)
{
    static char buf[8];
    for (const auto& kn : kKeyNames)
    {
        if (kn.key == key)
            return kn.name;
    }
    if (key > 0 && key < 0x80 && !(key & SDLK_SCANCODE_MASK))
    {
        buf[0] = static_cast<char>(std::toupper(key));
        buf[1] = 0;
        return buf;
    }
    if (key >= 0x80 && key < 0x100)
    {
        // Latin-1 as UTF-8
        buf[0] = static_cast<char>(0xC0 | (key >> 6));
        buf[1] = static_cast<char>(0x80 | (key & 0x3F));
        buf[2] = 0;
        return buf;
    }
    buf[0] = 0;
    return buf;
}

SDL_Keycode SDL_GetKeyFromName(const char* name)
{
    if (name == nullptr || *name == 0)
        return SDLK_UNKNOWN;
    for (const auto& kn : kKeyNames)
    {
        if (IEquals(kn.name, name))
            return kn.key;
    }
    // OpenRCT2's default bindings spell the keypad "NUMPAD n"
    if ((std::strncmp(name, "NUMPAD ", 7) == 0 || std::strncmp(name, "Numpad ", 7) == 0) && name[7] != 0)
    {
        std::string alt = std::string("Keypad ") + (name + 7);
        for (const auto& kn : kKeyNames)
        {
            if (IEquals(kn.name, alt.c_str()))
                return kn.key;
        }
    }
    if (IEquals(name, "PGUP"))
        return SDLK_PAGEUP;
    if (IEquals(name, "PGDN"))
        return SDLK_PAGEDOWN;
    if (IEquals(name, "SPACEBAR"))
        return SDLK_SPACE;
    if (IEquals(name, "ENTER"))
        return SDLK_RETURN;
    if (IEquals(name, "ESC"))
        return SDLK_ESCAPE;
    if (name[1] == 0)
        return static_cast<SDL_Keycode>(std::tolower(static_cast<unsigned char>(name[0])));
    return SDLK_UNKNOWN;
}

void SDL_StartTextInput(void)
{
    g_textInput = true;
}
void SDL_StopTextInput(void)
{
    g_textInput = false;
}
SDL_bool SDL_IsTextInputActive(void)
{
    return g_textInput ? SDL_TRUE : SDL_FALSE;
}
void SDL_SetTextInputRect(const SDL_Rect*)
{
}

// Cursors: the Intuition pointer stays; the game's cursor shapes are accepted and ignored for now.
struct SDL_Cursor
{
    int id;
};
SDL_Cursor* SDL_CreateCursor(const Uint8*, const Uint8*, int, int, int, int)
{
    return new SDL_Cursor{ 0 };
}
SDL_Cursor* SDL_CreateSystemCursor(SDL_SystemCursor id)
{
    return new SDL_Cursor{ static_cast<int>(id) };
}
void SDL_SetCursor(SDL_Cursor*)
{
}
void SDL_FreeCursor(SDL_Cursor* cursor)
{
    delete cursor;
}

int SDL_SetClipboardText(const char*)
{
    return -1;
}
char* SDL_GetClipboardText(void)
{
    char* s = static_cast<char*>(std::malloc(1));
    if (s != nullptr)
        *s = 0;
    return s;
}
SDL_bool SDL_HasClipboardText(void)
{
    return SDL_FALSE;
}

int SDL_ShowMessageBox(const SDL_MessageBoxData* data, int* buttonid)
{
    std::string gadgets;
    for (int i = 0; i < data->numbuttons; i++)
    {
        if (i > 0)
            gadgets += '|';
        gadgets += data->buttons[i].text;
    }
    if (gadgets.empty())
        gadgets = "OK";
    int r = amiga_ui_request(data->title != nullptr ? data->title : "OpenRCT2", data->message != nullptr ? data->message : "", gadgets.c_str());
    if (buttonid != nullptr)
        *buttonid = (r >= 0 && r < data->numbuttons) ? data->buttons[r].buttonid : 0;
    return 0;
}
int SDL_ShowSimpleMessageBox(Uint32, const char* title, const char* message, SDL_Window*)
{
    amiga_ui_request(title != nullptr ? title : "OpenRCT2", message != nullptr ? message : "", "OK");
    return 0;
}

int SDL_NumJoysticks(void)
{
    return 0;
}
SDL_bool SDL_IsGameController(int)
{
    return SDL_FALSE;
}
SDL_GameController* SDL_GameControllerOpen(int)
{
    return nullptr;
}
SDL_Joystick* SDL_GameControllerGetJoystick(SDL_GameController*)
{
    return nullptr;
}
Sint16 SDL_GameControllerGetAxis(SDL_GameController*, SDL_GameControllerAxis)
{
    return 0;
}
Uint8 SDL_JoystickGetButton(SDL_Joystick*, int)
{
    return 0;
}
int SDL_JoystickNumHats(SDL_Joystick*)
{
    return 0;
}
Uint8 SDL_JoystickGetHat(SDL_Joystick*, int)
{
    return SDL_HAT_CENTERED;
}

} // extern "C"

#endif // __amigaos__
