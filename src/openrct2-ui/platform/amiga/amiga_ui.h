/*****************************************************************************
 * Plain-C AmigaOS display/input layer for the openrct2-ui SDL2 shim.
 * Kept in C so the NDK headers never meet the C++ tree (same rule as amiga_os.c).
 *****************************************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    AMIGA_UI_EV_NONE = 0,
    AMIGA_UI_EV_RAWKEY = 1,      /* code: raw key (bit 7 = up), qual: IEQUALIFIER bits, ascii: mapped char or -1 */
    AMIGA_UI_EV_MOUSEMOVE = 2,   /* x, y */
    AMIGA_UI_EV_BUTTON = 3,      /* code: 1 left 2 middle 3 right; qual bit 15 = down; x, y */
    AMIGA_UI_EV_WHEEL = 4,       /* code: +1 up, -1 down */
    AMIGA_UI_EV_ACTIVE = 5,      /* code: 1 activated, 0 deactivated */
    AMIGA_UI_EV_CLOSE = 6,
};

typedef struct amiga_ui_event
{
    int type;
    int code;
    int qual;
    int x;
    int y;
    int ascii;
} amiga_ui_event;

/* Opens an 8-bit RTG screen of (at least) the requested size plus a backdrop window.
 * Returns 1 and the actual size on success. */
int amiga_ui_open(int width, int height, const char* title, int* outWidth, int* outHeight);
void amiga_ui_close(void);
int amiga_ui_is_open(void);

/* 256 entries of r,g,b (768 bytes). */
void amiga_ui_set_palette(const unsigned char* rgb);
/* Copies a chunky 8-bit rectangle (source pitch in bytes) to screen coordinates x,y. */
void amiga_ui_blit(const unsigned char* src, int srcPitch, int x, int y, int w, int h);

/* Non-blocking; returns 1 when an event was written. */
int amiga_ui_poll(amiga_ui_event* ev);
/* Raw key + qualifier -> characters (keymap.library). Returns the count, 0 for none. */
int amiga_ui_map_key(int code, int qual, char* out, int outSize);
/* Current pointer position (window relative) and button mask (bit0 left, bit1 middle, bit2 right). */
int amiga_ui_mouse(int* x, int* y);
void amiga_ui_show_pointer(int show);

/* EasyRequest with '|' separated gadgets; returns the pressed gadget index (0-based, left to right). */
int amiga_ui_request(const char* title, const char* body, const char* gadgets);

/* 1 if an 8-bit RTG mode of that size exists. */
int amiga_ui_mode_available(int width, int height);
/* Workbench (default public screen) size. */
int amiga_ui_desktop_size(int* width, int* height);

#ifdef __cplusplus
}
#endif
