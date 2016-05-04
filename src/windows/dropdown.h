#ifndef _WINDOW_DROPDOWN_H_
#define _WINDOW_DROPDOWN_H_

#include "../common.h"

#define DROPDOWN_SEPARATOR 0

enum
{
	DROPDOWN_FLAG_STAY_OPEN = (1 << 7)
};

extern int gAppropriateImageDropdownItemsPerRow[];

extern int gDropdownNumItems;
extern uint16 gDropdownItemsFormat[64];
extern sint64 gDropdownItemsArgs[64];
extern uint64 gDropdownItemsChecked;
extern uint64 gDropdownItemsDisabled;
extern bool gDropdownIsColour;
extern int gDropdownLastColourHover;
extern int gDropdownHighlightedIndex;
extern int gDropdownDefaultIndex;

bool dropdown_is_checked(int index);
bool dropdown_is_disabled(int index);
void dropdown_set_checked(int index, bool value);
void dropdown_set_disabled(int index, bool value);

void window_dropdown_show_text(int x, int y, int extray, uint8 colour, uint8 flags, int num_items);
void window_dropdown_show_text_custom_width(int x, int y, int extray, uint8 colour, uint8 flags, int num_items, int width);
void window_dropdown_show_image(int x, int y, int extray, uint8 colour, uint8 flags, int numItems, int itemWidth, int itemHeight, int numColumns);
void window_dropdown_close();
int dropdown_index_from_point(int x, int y, rct_window* w);
void window_dropdown_show_colour(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour);
void window_dropdown_show_colour_available(rct_window *w, rct_widget *widget, uint8 dropdownColour, uint8 selectedColour, uint32 availableColours);

#endif
