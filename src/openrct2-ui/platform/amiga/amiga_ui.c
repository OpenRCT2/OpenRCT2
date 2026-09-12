/*****************************************************************************
 * AmigaOS display/input layer for the openrct2-ui SDL2 shim (plain C).
 * 8-bit CyberGraphX/Picasso96 screen, backdrop window, IDCMP input.
 *****************************************************************************/
#ifdef __amigaos__

    #include "amiga_ui.h"

    #include <cybergraphx/cybergraphics.h>
    #include <devices/inputevent.h>
    #include <graphics/displayinfo.h>
    #include <intuition/intuition.h>
    #include <intuition/screens.h>
    #include <proto/cybergraphics.h>
    #include <proto/dos.h>
    #include <proto/exec.h>
    #include <proto/graphics.h>
    #include <proto/intuition.h>
    #include <proto/keymap.h>
    #include <stdio.h>
    #include <string.h>

extern void amiga_trace(const char* line);
static void trace(const char* fmt, long a, long b, long c, long d)
{
    char buf[160];
    snprintf(buf, sizeof buf, fmt, a, b, c, d);
    amiga_trace(buf);
}

struct Library* CyberGfxBase = NULL;
struct Library* KeymapBase = NULL;

static struct Screen* s_screen = NULL;
static struct Window* s_window = NULL;
static int s_width = 0, s_height = 0;
static int s_mouseX = 0, s_mouseY = 0, s_buttons = 0;
static ULONG s_palette[1 + 256 * 3 + 1];

static int open_libs(void)
{
    if (CyberGfxBase == NULL)
        CyberGfxBase = OpenLibrary("cybergraphics.library", 41);
    if (KeymapBase == NULL)
        KeymapBase = OpenLibrary("keymap.library", 36);
    return CyberGfxBase != NULL;
}

/* Smallest 8-bit RTG mode that fits width x height; the largest one if none fits. */
static ULONG find_mode(int width, int height)
{
    ULONG id = INVALID_ID, best = INVALID_ID, largest = INVALID_ID;
    long bestArea = -1, largestArea = -1;
    if (!open_libs())
        return INVALID_ID;
    while ((id = NextDisplayInfo(id)) != (ULONG)INVALID_ID)
    {
        long w, h, area;
        if (!IsCyberModeID(id) || GetCyberIDAttr(CYBRIDATTR_DEPTH, id) != 8)
            continue;
        w = (long)GetCyberIDAttr(CYBRIDATTR_WIDTH, id);
        h = (long)GetCyberIDAttr(CYBRIDATTR_HEIGHT, id);
        area = w * h;
        if (area > largestArea)
        {
            largestArea = area;
            largest = id;
        }
        if (w >= width && h >= height && (bestArea < 0 || area < bestArea))
        {
            bestArea = area;
            best = id;
        }
    }
    trace("ui: find_mode %ldx%ld -> best %08lx largest %08lx", width, height, (long)best, (long)largest);
    return best != (ULONG)INVALID_ID ? best : largest;
}

int amiga_ui_mode_available(int width, int height)
{
    ULONG id = find_mode(width, height);
    if (id == (ULONG)INVALID_ID)
        return 0;
    return GetCyberIDAttr(CYBRIDATTR_WIDTH, id) == (ULONG)width && GetCyberIDAttr(CYBRIDATTR_HEIGHT, id) == (ULONG)height;
}

int amiga_ui_desktop_size(int* width, int* height)
{
    struct Screen* wb = LockPubScreen(NULL);
    if (wb == NULL)
        return 0;
    *width = wb->Width;
    *height = wb->Height;
    UnlockPubScreen(NULL, wb);
    return 1;
}

int amiga_ui_open(int width, int height, const char* title, int* outWidth, int* outHeight)
{
    ULONG modeId;
    amiga_ui_close();
    modeId = find_mode(width, height);
    if (modeId == (ULONG)INVALID_ID)
        return 0;
    {
        /* open the screen at the mode's own size: no autoscroll, and the game adapts to the size it gets */
        int mw = (int)GetCyberIDAttr(CYBRIDATTR_WIDTH, modeId);
        int mh = (int)GetCyberIDAttr(CYBRIDATTR_HEIGHT, modeId);
        trace("ui: open requested %ldx%ld mode %ldx%ld", width, height, mw, mh);
        width = mw;
        height = mh;
    }
    s_screen = OpenScreenTags(
        NULL, SA_DisplayID, modeId, SA_Width, width, SA_Height, height, SA_Depth, 8, SA_Title, (ULONG)title, SA_Quiet, TRUE,
        SA_ShowTitle, FALSE, SA_Behind, FALSE, TAG_DONE);
    if (s_screen == NULL)
        return 0;
    s_window = OpenWindowTags(
        NULL, WA_CustomScreen, (ULONG)s_screen, WA_Left, 0, WA_Top, 0, WA_Width, s_screen->Width, WA_Height, s_screen->Height,
        WA_Backdrop, TRUE, WA_Borderless, TRUE, WA_Activate, TRUE, WA_RMBTrap, TRUE, WA_NoCareRefresh, TRUE, WA_SimpleRefresh,
        TRUE, WA_ReportMouse, TRUE, WA_IDCMP,
        IDCMP_RAWKEY | IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW, TAG_DONE);
    if (s_window == NULL)
    {
        CloseScreen(s_screen);
        s_screen = NULL;
        return 0;
    }
    s_width = s_screen->Width;
    s_height = s_screen->Height;
    *outWidth = s_width;
    *outHeight = s_height;
    trace("ui: screen %ldx%ld depth %ld open", s_width, s_height, s_screen->RastPort.BitMap->Depth, 0);
    return 1;
}

void amiga_ui_close(void)
{
    if (s_window != NULL)
    {
        CloseWindow(s_window);
        s_window = NULL;
    }
    if (s_screen != NULL)
    {
        CloseScreen(s_screen);
        s_screen = NULL;
    }
}

int amiga_ui_is_open(void)
{
    return s_window != NULL;
}

void amiga_ui_set_palette(const unsigned char* rgb)
{
    int i;
    if (s_screen == NULL)
        return;
    s_palette[0] = (256UL << 16) | 0;
    for (i = 0; i < 256; i++)
    {
        s_palette[1 + i * 3 + 0] = (ULONG)rgb[i * 3 + 0] * 0x01010101UL;
        s_palette[1 + i * 3 + 1] = (ULONG)rgb[i * 3 + 1] * 0x01010101UL;
        s_palette[1 + i * 3 + 2] = (ULONG)rgb[i * 3 + 2] * 0x01010101UL;
    }
    s_palette[1 + 256 * 3] = 0;
    LoadRGB32(&s_screen->ViewPort, s_palette);
}

void amiga_ui_blit(const unsigned char* src, int srcPitch, int x, int y, int w, int h)
{
    if (s_window == NULL || w <= 0 || h <= 0)
        return;
    if (x < 0 || y < 0 || x + w > s_width || y + h > s_height)
        return;
    WriteChunkyPixels(s_window->RPort, x, y, x + w - 1, y + h - 1, (UBYTE*)src, srcPitch);
}

int amiga_ui_poll(amiga_ui_event* ev)
{
    struct IntuiMessage* m;
    if (s_window == NULL)
        return 0;
    while ((m = (struct IntuiMessage*)GetMsg(s_window->UserPort)) != NULL)
    {
        ULONG cls = m->Class;
        UWORD code = m->Code;
        UWORD qual = m->Qualifier;
        int mx = m->MouseX, my = m->MouseY;
        APTR iaddr = m->IAddress;
        int handled = 1;
        memset(ev, 0, sizeof(*ev));
        switch (cls)
        {
            case IDCMP_RAWKEY:
                if ((code & 0x7F) == 0x7A || (code & 0x7F) == 0x7B)
                {
                    /* NewMouse wheel codes */
                    if (code & 0x80)
                        handled = 0;
                    else
                    {
                        ev->type = AMIGA_UI_EV_WHEEL;
                        ev->code = (code == 0x7A) ? 1 : -1;
                    }
                }
                else
                {
                    struct InputEvent ie;
                    char buf[4];
                    LONG n;
                    memset(&ie, 0, sizeof ie);
                    ie.ie_Class = IECLASS_RAWKEY;
                    ie.ie_Code = code;
                    ie.ie_Qualifier = qual;
                    ie.ie_EventAddress = *((APTR*)iaddr);
                    ev->type = AMIGA_UI_EV_RAWKEY;
                    ev->code = code;
                    ev->qual = qual;
                    ev->ascii = -1;
                    if (KeymapBase != NULL && !(code & 0x80))
                    {
                        n = MapRawKey(&ie, buf, sizeof buf, NULL);
                        if (n == 1)
                            ev->ascii = (unsigned char)buf[0];
                    }
                }
                break;
            case IDCMP_MOUSEMOVE:
                ev->type = AMIGA_UI_EV_MOUSEMOVE;
                ev->x = s_mouseX = mx;
                ev->y = s_mouseY = my;
                break;
            case IDCMP_MOUSEBUTTONS:
            {
                int button = 0, down = 0;
                switch (code)
                {
                    case SELECTDOWN:
                        button = 1;
                        down = 1;
                        break;
                    case SELECTUP:
                        button = 1;
                        break;
                    case MENUDOWN:
                        button = 3;
                        down = 1;
                        break;
                    case MENUUP:
                        button = 3;
                        break;
                    case MIDDLEDOWN:
                        button = 2;
                        down = 1;
                        break;
                    case MIDDLEUP:
                        button = 2;
                        break;
                    default:
                        handled = 0;
                        break;
                }
                if (handled)
                {
                    ev->type = AMIGA_UI_EV_BUTTON;
                    ev->code = button;
                    ev->qual = down ? 0x8000 : 0;
                    ev->x = s_mouseX = mx;
                    ev->y = s_mouseY = my;
                    if (down)
                        s_buttons |= 1 << (button - 1);
                    else
                        s_buttons &= ~(1 << (button - 1));
                }
                break;
            }
            case IDCMP_ACTIVEWINDOW:
                ev->type = AMIGA_UI_EV_ACTIVE;
                ev->code = 1;
                break;
            case IDCMP_INACTIVEWINDOW:
                ev->type = AMIGA_UI_EV_ACTIVE;
                ev->code = 0;
                break;
            default:
                handled = 0;
                break;
        }
        ReplyMsg((struct Message*)m);
        if (handled)
            return 1;
    }
    return 0;
}

int amiga_ui_map_key(int code, int qual, char* out, int outSize)
{
    struct InputEvent ie;
    LONG n;
    if (KeymapBase == NULL)
        return 0;
    memset(&ie, 0, sizeof ie);
    ie.ie_Class = IECLASS_RAWKEY;
    ie.ie_Code = (UWORD)code;
    ie.ie_Qualifier = (UWORD)qual;
    n = MapRawKey(&ie, out, outSize, NULL);
    return n < 0 ? 0 : (int)n;
}

int amiga_ui_mouse(int* x, int* y)
{
    if (s_window != NULL)
    {
        s_mouseX = s_window->MouseX;
        s_mouseY = s_window->MouseY;
    }
    if (x != NULL)
        *x = s_mouseX;
    if (y != NULL)
        *y = s_mouseY;
    return s_buttons;
}

void amiga_ui_show_pointer(int show)
{
    static UWORD blank[6] = { 0, 0, 0, 0, 0, 0 };
    if (s_window == NULL)
        return;
    if (show)
        ClearPointer(s_window);
    else
        SetPointer(s_window, blank, 1, 16, 0, 0);
}

int amiga_ui_request(const char* title, const char* body, const char* gadgets)
{
    struct EasyStruct es;
    LONG r;
    int n = 1;
    const char* p;
    for (p = gadgets; *p; p++)
        if (*p == '|')
            n++;
    es.es_StructSize = sizeof(es);
    es.es_Flags = 0;
    es.es_Title = (UBYTE*)title;
    es.es_TextFormat = (UBYTE*)"%s";
    es.es_GadgetFormat = (UBYTE*)gadgets;
    r = EasyRequest(s_window, &es, NULL, body);
    /* EasyRequest: 1..n-1 for the left gadgets, 0 for the rightmost */
    if (r == 0)
        return n - 1;
    return (int)r - 1;
}

#endif
