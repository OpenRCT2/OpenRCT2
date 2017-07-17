#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/string_ids.h"
#include "../sprites.h"

static rct_widget window_widgets_tabs_widgets[] = {
        {WWT_FRAME,  0, 0, 128 - 1, 0, 81 - 1, STR_NONE, STR_NONE},
        {WIDGETS_END},
};

static void window_widgets_tabs_update(rct_window *w) ;
static void window_widgets_tabs_paint(rct_window *w, rct_drawpixelinfo *dpi) ;

static rct_window_event_list window_widgets_tabs_events = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        window_widgets_tabs_update,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        window_widgets_tabs_paint,
        NULL
};


typedef struct tab_animation {
    int fps;
    int count;
    int images[];
} tab_animation;

#define MAKE_TABS(count, ...)   { count, countof(((int[]){__VA_ARGS__})), {__VA_ARGS__}}


const tab_animation TAB_FINANCES_SUMMARY = MAKE_TABS(
        2,
        SPR_TAB_FINANCES_SUMMARY_0,
        SPR_TAB_FINANCES_SUMMARY_1,
        SPR_TAB_FINANCES_SUMMARY_2,
        SPR_TAB_FINANCES_SUMMARY_3,
        SPR_TAB_FINANCES_SUMMARY_4,
        SPR_TAB_FINANCES_SUMMARY_5,
        SPR_TAB_FINANCES_SUMMARY_6,
        SPR_TAB_FINANCES_SUMMARY_7,
);

const tab_animation TAB_GUESTS = MAKE_TABS(
        3,
        SPR_TAB_GUESTS_0,
        SPR_TAB_GUESTS_1,
        SPR_TAB_GUESTS_2,
        SPR_TAB_GUESTS_3,
        SPR_TAB_GUESTS_4,
        SPR_TAB_GUESTS_5,
        SPR_TAB_GUESTS_6,
        SPR_TAB_GUESTS_7,
);

const tab_animation TAB_RIDE = MAKE_TABS(
        4,
        SPR_TAB_RIDE_0,
        SPR_TAB_RIDE_1,
        SPR_TAB_RIDE_2,
        SPR_TAB_RIDE_3,
        SPR_TAB_RIDE_4,
        SPR_TAB_RIDE_5,
        SPR_TAB_RIDE_6,
        SPR_TAB_RIDE_7,
        SPR_TAB_RIDE_8,
        SPR_TAB_RIDE_9,
        SPR_TAB_RIDE_10,
        SPR_TAB_RIDE_11,
        SPR_TAB_RIDE_12,
        SPR_TAB_RIDE_13,
        SPR_TAB_RIDE_14,
        SPR_TAB_RIDE_15,
);

const tab_animation TAB_THOUGHTS = MAKE_TABS(
        2,
        SPR_TAB_THOUGHTS_0,
        SPR_TAB_THOUGHTS_1,
        SPR_TAB_THOUGHTS_2,
        SPR_TAB_THOUGHTS_3,
        SPR_TAB_THOUGHTS_4,
        SPR_TAB_THOUGHTS_5,
        SPR_TAB_THOUGHTS_6,
        SPR_TAB_THOUGHTS_7,
);

tab_animation TAB_STATS = MAKE_TABS(
        4,
        SPR_TAB_STATS_0,
        SPR_TAB_STATS_1,
        SPR_TAB_STATS_2,
        SPR_TAB_STATS_3,
        SPR_TAB_STATS_4,
        SPR_TAB_STATS_5,
        SPR_TAB_STATS_6,
);


tab_animation TAB_OBJECTIVE = MAKE_TABS(
        4,
        SPR_TAB_OBJECTIVE_0,
        SPR_TAB_OBJECTIVE_1,
        SPR_TAB_OBJECTIVE_2,
        SPR_TAB_OBJECTIVE_3,
        SPR_TAB_OBJECTIVE_4,
        SPR_TAB_OBJECTIVE_5,
        SPR_TAB_OBJECTIVE_6,
        SPR_TAB_OBJECTIVE_7,
        SPR_TAB_OBJECTIVE_8,
        SPR_TAB_OBJECTIVE_9,
        SPR_TAB_OBJECTIVE_10,
        SPR_TAB_OBJECTIVE_11,
        SPR_TAB_OBJECTIVE_12,
        SPR_TAB_OBJECTIVE_13,
        SPR_TAB_OBJECTIVE_14,
        SPR_TAB_OBJECTIVE_15,
);

tab_animation TAB_PAINT = MAKE_TABS(
        4,
        SPR_TAB_PAINT_0,
        SPR_TAB_PAINT_1,
        SPR_TAB_PAINT_2,
        SPR_TAB_PAINT_3,
        SPR_TAB_PAINT_4,
        SPR_TAB_PAINT_5,
        SPR_TAB_PAINT_6,
        SPR_TAB_PAINT_7,
);

tab_animation TAB_TIMER = MAKE_TABS(
        8,
        SPR_TAB_TIMER_0,
        SPR_TAB_TIMER_1,
        SPR_TAB_TIMER_2,
        SPR_TAB_TIMER_3,
        SPR_TAB_TIMER_4,
        SPR_TAB_TIMER_5,
        SPR_TAB_TIMER_6,
        SPR_TAB_TIMER_7,
);


tab_animation TAB_MUSIC = MAKE_TABS(
        2,
        SPR_TAB_MUSIC_0,
        SPR_TAB_MUSIC_1,
        SPR_TAB_MUSIC_2,
        SPR_TAB_MUSIC_3,
        SPR_TAB_MUSIC_4,
        SPR_TAB_MUSIC_5,
        SPR_TAB_MUSIC_6,
        SPR_TAB_MUSIC_7,
        SPR_TAB_MUSIC_8,
        SPR_TAB_MUSIC_9,
        SPR_TAB_MUSIC_10,
        SPR_TAB_MUSIC_11,
        SPR_TAB_MUSIC_12,
        SPR_TAB_MUSIC_13,
        SPR_TAB_MUSIC_14,
        SPR_TAB_MUSIC_15,
);

const tab_animation TAB_GEARS = MAKE_TABS(
        2,
        SPR_TAB_GEARS_0,
        SPR_TAB_GEARS_1,
        SPR_TAB_GEARS_2,
        SPR_TAB_GEARS_3,
);
const tab_animation TAB_WRENCH = MAKE_TABS(
        2,
        SPR_TAB_WRENCH_0,
        SPR_TAB_WRENCH_1,
        SPR_TAB_WRENCH_2,
        SPR_TAB_WRENCH_3,
        SPR_TAB_WRENCH_4,
        SPR_TAB_WRENCH_5,
        SPR_TAB_WRENCH_6,
        SPR_TAB_WRENCH_7,
        SPR_TAB_WRENCH_8,
        SPR_TAB_WRENCH_9,
        SPR_TAB_WRENCH_10,
        SPR_TAB_WRENCH_11,
        SPR_TAB_WRENCH_12,
        SPR_TAB_WRENCH_13,
        SPR_TAB_WRENCH_14,
        SPR_TAB_WRENCH_15,
);

const tab_animation TAB_GRAPH_A = MAKE_TABS(
        8,
        SPR_TAB_GRAPH_A_0,
        SPR_TAB_GRAPH_A_1,
        SPR_TAB_GRAPH_A_2,
        SPR_TAB_GRAPH_A_3,
        SPR_TAB_GRAPH_A_4,
        SPR_TAB_GRAPH_A_5,
        SPR_TAB_GRAPH_A_6,
        SPR_TAB_GRAPH_A_7,
);

const tab_animation TAB_GRAPH = MAKE_TABS(
        8,
        SPR_TAB_GRAPH_0,
        SPR_TAB_GRAPH_1,
        SPR_TAB_GRAPH_2,
        SPR_TAB_GRAPH_3,
        SPR_TAB_GRAPH_4,
        SPR_TAB_GRAPH_5,
        SPR_TAB_GRAPH_6,
        SPR_TAB_GRAPH_7,
);

const tab_animation TAB_ADMISSION = MAKE_TABS(
        2,
        SPR_TAB_ADMISSION_0,
        SPR_TAB_ADMISSION_1,
        SPR_TAB_ADMISSION_2,
        SPR_TAB_ADMISSION_3,
        SPR_TAB_ADMISSION_4,
        SPR_TAB_ADMISSION_5,
        SPR_TAB_ADMISSION_6,
        SPR_TAB_ADMISSION_7,
);

const tab_animation TAB_PEEP_LARGE_FACE_VERY_SICK = MAKE_TABS(
        8,
        SPR_PEEP_LARGE_FACE_VERY_SICK_0,
        SPR_PEEP_LARGE_FACE_VERY_SICK_1,
        SPR_PEEP_LARGE_FACE_VERY_SICK_2,
        SPR_PEEP_LARGE_FACE_VERY_SICK_3,
);

const tab_animation TAB_PEEP_LARGE_FACE_VERY_VERY_SICK = MAKE_TABS(
        4,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_0,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_1,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_2,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_3,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_4,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_5,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_6,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_7,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_8,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_9,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_10,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_11,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_12,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_13,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_14,
        SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_15,
);

const tab_animation TAB_PEEP_LARGE_FACE_ANGRY = MAKE_TABS(
        8,
        SPR_PEEP_LARGE_FACE_ANGRY_0,
        SPR_PEEP_LARGE_FACE_ANGRY_1,
        SPR_PEEP_LARGE_FACE_ANGRY_2,
        SPR_PEEP_LARGE_FACE_ANGRY_3,
);

const tab_animation TAB_STAFF_OPTIONS = MAKE_TABS(
        2,
        SPR_TAB_STAFF_OPTIONS_0,
        SPR_TAB_STAFF_OPTIONS_1,
        SPR_TAB_STAFF_OPTIONS_2,
        SPR_TAB_STAFF_OPTIONS_3,
        SPR_TAB_STAFF_OPTIONS_4,
        SPR_TAB_STAFF_OPTIONS_5,
        SPR_TAB_STAFF_OPTIONS_6,
);

const tab_animation TAB_SHOPS_AND_STALLS = MAKE_TABS(
        4,
        SPR_TAB_SHOPS_AND_STALLS_0,
        SPR_TAB_SHOPS_AND_STALLS_1,
        SPR_TAB_SHOPS_AND_STALLS_2,
        SPR_TAB_SHOPS_AND_STALLS_3,
        SPR_TAB_SHOPS_AND_STALLS_4,
        SPR_TAB_SHOPS_AND_STALLS_5,
        SPR_TAB_SHOPS_AND_STALLS_6,
        SPR_TAB_SHOPS_AND_STALLS_7,
        SPR_TAB_SHOPS_AND_STALLS_8,
        SPR_TAB_SHOPS_AND_STALLS_9,
        SPR_TAB_SHOPS_AND_STALLS_10,
        SPR_TAB_SHOPS_AND_STALLS_11,
        SPR_TAB_SHOPS_AND_STALLS_12,
        SPR_TAB_SHOPS_AND_STALLS_13,
        SPR_TAB_SHOPS_AND_STALLS_14,
        SPR_TAB_SHOPS_AND_STALLS_15,
);

const tab_animation TAB_KIOSKS_AND_FACILITIES = MAKE_TABS(
        4,
        SPR_TAB_KIOSKS_AND_FACILITIES_0,
        SPR_TAB_KIOSKS_AND_FACILITIES_1,
        SPR_TAB_KIOSKS_AND_FACILITIES_2,
        SPR_TAB_KIOSKS_AND_FACILITIES_3,
        SPR_TAB_KIOSKS_AND_FACILITIES_4,
        SPR_TAB_KIOSKS_AND_FACILITIES_5,
        SPR_TAB_KIOSKS_AND_FACILITIES_6,
        SPR_TAB_KIOSKS_AND_FACILITIES_7,
);

const tab_animation TAB_RIDES_THRILL = MAKE_TABS(
        4,
        SPR_TAB_RIDES_THRILL_5,
        SPR_TAB_RIDES_THRILL_6,
        SPR_TAB_RIDES_THRILL_5,
        SPR_TAB_RIDES_THRILL_4,
        SPR_TAB_RIDES_THRILL_3,
        SPR_TAB_RIDES_THRILL_2,
        SPR_TAB_RIDES_THRILL_1,
        SPR_TAB_RIDES_THRILL_0,
        SPR_TAB_RIDES_THRILL_0,
        SPR_TAB_RIDES_THRILL_0,
        SPR_TAB_RIDES_THRILL_0,
        SPR_TAB_RIDES_THRILL_0,
        SPR_TAB_RIDES_THRILL_0,
        SPR_TAB_RIDES_THRILL_0,
        SPR_TAB_RIDES_THRILL_1,
        SPR_TAB_RIDES_THRILL_2,
        SPR_TAB_RIDES_THRILL_3,
        SPR_TAB_RIDES_THRILL_4,
);

const tab_animation TAB_RIDES_TRANSPORT = MAKE_TABS(
        4,
        SPR_TAB_RIDES_TRANSPORT_0 | 0x20000000 | (COLOUR_WHITE << 19),
        SPR_TAB_RIDES_TRANSPORT_1 | 0x20000000 | (COLOUR_WHITE << 19),
        SPR_TAB_RIDES_TRANSPORT_2 | 0x20000000 | (COLOUR_WHITE << 19),
        SPR_TAB_RIDES_TRANSPORT_3 | 0x20000000 | (COLOUR_WHITE << 19),
        SPR_TAB_RIDES_TRANSPORT_4 | 0x20000000 | (COLOUR_WHITE << 19),
);

const tab_animation TAB_RIDES_GENTLE = MAKE_TABS(
        8,
        SPR_TAB_RIDES_GENTLE_0,
        SPR_TAB_RIDES_GENTLE_1,
        SPR_TAB_RIDES_GENTLE_2,
        SPR_TAB_RIDES_GENTLE_3,
);

const tab_animation TAB_RIDES_ROLLER_COASTERS = MAKE_TABS(
        2,
        SPR_TAB_RIDES_ROLLER_COASTERS_0 | 0x20000000 | (COLOUR_WHITE << 19),
        SPR_TAB_RIDES_ROLLER_COASTERS_1 | 0x20000000 | (COLOUR_WHITE << 19),
        SPR_TAB_RIDES_ROLLER_COASTERS_2 | 0x20000000 | (COLOUR_WHITE << 19),
        SPR_TAB_RIDES_ROLLER_COASTERS_3 | 0x20000000 | (COLOUR_WHITE << 19),
        SPR_TAB_RIDES_ROLLER_COASTERS_4 | 0x20000000 | (COLOUR_WHITE << 19),
);

const tab_animation TAB_RIDES_WATER = MAKE_TABS(
        4,
        SPR_TAB_RIDES_WATER_0,
        SPR_TAB_RIDES_WATER_1,
        SPR_TAB_RIDES_WATER_2,
        SPR_TAB_RIDES_WATER_3,
        SPR_TAB_RIDES_WATER_4,
        SPR_TAB_RIDES_WATER_5,
);

const tab_animation TAB_RIDES_SHOP = MAKE_TABS(
        4,
        SPR_TAB_RIDES_SHOP_0,
        SPR_TAB_RIDES_SHOP_1,
        SPR_TAB_RIDES_SHOP_2,
        SPR_TAB_RIDES_SHOP_3,
        SPR_TAB_RIDES_SHOP_4,
        SPR_TAB_RIDES_SHOP_5,
        SPR_TAB_RIDES_SHOP_6,
);

const tab_animation TAB_FINANCES_RESEARCH = MAKE_TABS(
        2,
        SPR_TAB_FINANCES_RESEARCH_0,
        SPR_TAB_FINANCES_RESEARCH_1,
        SPR_TAB_FINANCES_RESEARCH_2,
        SPR_TAB_FINANCES_RESEARCH_3,
        SPR_TAB_FINANCES_RESEARCH_4,
        SPR_TAB_FINANCES_RESEARCH_5,
        SPR_TAB_FINANCES_RESEARCH_6,
        SPR_TAB_FINANCES_RESEARCH_7,
);


const tab_animation TAB_FINANCES_FINANCIAL_GRAPH = MAKE_TABS(
        2,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_0,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_1,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_2,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_3,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_4,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_5,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_6,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_7,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_8,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_9,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_10,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_11,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_12,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_13,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_14,
        SPR_TAB_FINANCES_FINANCIAL_GRAPH_15,
);

const tab_animation TAB_FINANCES_PROFIT_GRAPH = MAKE_TABS(
        2,
        SPR_TAB_FINANCES_PROFIT_GRAPH_0,
        SPR_TAB_FINANCES_PROFIT_GRAPH_1,
        SPR_TAB_FINANCES_PROFIT_GRAPH_2,
        SPR_TAB_FINANCES_PROFIT_GRAPH_3,
        SPR_TAB_FINANCES_PROFIT_GRAPH_4,
        SPR_TAB_FINANCES_PROFIT_GRAPH_5,
        SPR_TAB_FINANCES_PROFIT_GRAPH_6,
        SPR_TAB_FINANCES_PROFIT_GRAPH_7,
        SPR_TAB_FINANCES_PROFIT_GRAPH_8,
        SPR_TAB_FINANCES_PROFIT_GRAPH_9,
        SPR_TAB_FINANCES_PROFIT_GRAPH_10,
        SPR_TAB_FINANCES_PROFIT_GRAPH_11,
        SPR_TAB_FINANCES_PROFIT_GRAPH_12,
        SPR_TAB_FINANCES_PROFIT_GRAPH_13,
        SPR_TAB_FINANCES_PROFIT_GRAPH_14,
        SPR_TAB_FINANCES_PROFIT_GRAPH_15,
);

const tab_animation TAB_FINANCES_VALUE_GRAPH = MAKE_TABS(
        2,
        SPR_TAB_FINANCES_VALUE_GRAPH_0,
        SPR_TAB_FINANCES_VALUE_GRAPH_1,
        SPR_TAB_FINANCES_VALUE_GRAPH_2,
        SPR_TAB_FINANCES_VALUE_GRAPH_3,
        SPR_TAB_FINANCES_VALUE_GRAPH_4,
        SPR_TAB_FINANCES_VALUE_GRAPH_5,
        SPR_TAB_FINANCES_VALUE_GRAPH_6,
        SPR_TAB_FINANCES_VALUE_GRAPH_7,
        SPR_TAB_FINANCES_VALUE_GRAPH_8,
        SPR_TAB_FINANCES_VALUE_GRAPH_9,
        SPR_TAB_FINANCES_VALUE_GRAPH_10,
        SPR_TAB_FINANCES_VALUE_GRAPH_11,
        SPR_TAB_FINANCES_VALUE_GRAPH_12,
        SPR_TAB_FINANCES_VALUE_GRAPH_13,
        SPR_TAB_FINANCES_VALUE_GRAPH_14,
        SPR_TAB_FINANCES_VALUE_GRAPH_15,
);

const tab_animation TAB_FINANCES_MARKETING = MAKE_TABS(
        2,
        SPR_TAB_FINANCES_MARKETING_0,
        SPR_TAB_FINANCES_MARKETING_1,
        SPR_TAB_FINANCES_MARKETING_2,
        SPR_TAB_FINANCES_MARKETING_3,
        SPR_TAB_FINANCES_MARKETING_4,
        SPR_TAB_FINANCES_MARKETING_5,
        SPR_TAB_FINANCES_MARKETING_6,
        SPR_TAB_FINANCES_MARKETING_7,
        SPR_TAB_FINANCES_MARKETING_8,
        SPR_TAB_FINANCES_MARKETING_9,
        SPR_TAB_FINANCES_MARKETING_10,
        SPR_TAB_FINANCES_MARKETING_11,
        SPR_TAB_FINANCES_MARKETING_12,
        SPR_TAB_FINANCES_MARKETING_13,
        SPR_TAB_FINANCES_MARKETING_14,
        SPR_TAB_FINANCES_MARKETING_15,
        SPR_TAB_FINANCES_MARKETING_16,
        SPR_TAB_FINANCES_MARKETING_17,
        SPR_TAB_FINANCES_MARKETING_18,
);

const tab_animation *_animations[] = {
    /*  0 */ &TAB_GEARS,
    /*  1 */ &TAB_WRENCH,
    /*  2 */ &TAB_PAINT,
    /*  3 */ &TAB_TIMER,
    /*  4 */ &TAB_GRAPH_A,
    /*  5 */ &TAB_GRAPH,
    /*  6 */ &TAB_ADMISSION,
    /*  7 */ &TAB_FINANCES_SUMMARY,
    /*  8 */ &TAB_THOUGHTS,
    /*  9 */ &TAB_STATS,
    /* 10 */ &TAB_PEEP_LARGE_FACE_VERY_SICK,
    /* 11 */ &TAB_PEEP_LARGE_FACE_VERY_VERY_SICK,
    /* 12 */ &TAB_PEEP_LARGE_FACE_ANGRY,
    /* 13 */ &TAB_STAFF_OPTIONS,
    /* 14 */ &TAB_FINANCES_RESEARCH,
    /* 15 */ &TAB_MUSIC,
    /* 16 */ &TAB_SHOPS_AND_STALLS,
    /* 17 */ &TAB_KIOSKS_AND_FACILITIES,
    /* 18 */ &TAB_FINANCES_FINANCIAL_GRAPH,
    /* 19 */ &TAB_FINANCES_PROFIT_GRAPH,
    /* 20 */ &TAB_FINANCES_VALUE_GRAPH,
    /* 21 */ &TAB_FINANCES_MARKETING,
    /* 22 */ &TAB_RIDE,
    /* 23 */ &TAB_OBJECTIVE,
    /* 24 */ &TAB_RIDES_SHOP,
    /* 25 */ &TAB_RIDES_TRANSPORT,
    /* 26 */ &TAB_RIDES_GENTLE,
    /* 27 */ &TAB_RIDES_ROLLER_COASTERS,
    /* 28 */ &TAB_RIDES_WATER,
    /* 29 */ &TAB_RIDES_THRILL,
    /* 30 */ &TAB_GUESTS,
};

/**
 * Creates the window containing the menu buttons on the title screen.
 *  rct2: 0x0066B5C0 (part of 0x0066B3E8)
 */
void window_widgets_tabs_open() {
    rct_window *window;

    window = window_create(
            0, 0,
            0, 100,
            &window_widgets_tabs_events,
            WC_WIDGETS_TABS,
            WF_STICK_TO_FRONT
    );
    window->widgets = window_widgets_tabs_widgets;

    window->width = 4 + countof(_animations) * 33;
    window->height = 44;
    window->widgets[0].right = window->width-1;
    window->widgets[0].bottom = window->height-1;

/*
    int count = countof(_animations);

    bool usedSprites[324206] = {false};

    for (int i = 0; i < count; i++) {
        const tab_animation *animation = _animations[i];

        for (int n = 0; n < animation->count; n++) {
            usedSprites[animation->images[n]] = true;
        }
    }

    int sectionStart = -1;
    for (int i = 0; i < 324206; i++) {
        if (sectionStart == -1 && usedSprites[i] == false) {
            sectionStart = i;
            continue;
        }

        if (sectionStart != -1 && usedSprites[i] == true) {
            printf("%5i - %i\n", sectionStart, i - 1);
            sectionStart = -1;
            continue;
        }
    }

    exit(0);*/

/*
    int count = countof(_animations);

   // const_utf8string ** usedSprites = malloc(sizeof(uintptr_t) * 324206);

    for (int i = 0; i < count; i++) {
        const tab_animation *animation = _animations[i];

        int min = 324206;
        for (int n = 0; n < animation->count; n++) {
            min = min(min, animation->images[n] & 0x7FFFF);
        }

      //  (*usedSprites)[min] = _tabNames[i];
        printf("%d\t%s,\n",  min, _tabNames[i]);
    }

    exit(0);*/
}

static void window_widgets_tabs_update(rct_window *w) {
    // Tab animation
    w->frame_no++;
    widget_invalidate(w, 0);
}

static void window_widgets_tabs_paint(rct_window *w, rct_drawpixelinfo *dpi) {
    int count = countof(_animations);
    int x = w->x + 4;

    gfx_fill_rect(dpi, w->x, w->y, w->x + w->width, w->y + 44, ColourMapA[COLOUR_BLACK].mid_light);
    for (uint16 i = 0; i < count; i++) {
        const tab_animation *animation = _animations[i];
        gfx_fill_rect(dpi, x, w->y+4, x + 28, w->y + 28, ColourMapA[COLOUR_WHITE].lightest);
        gfx_draw_sprite(dpi, animation->images[(w->frame_no / animation->fps) % animation->count], x - 1, w->y +3, 0);

        gCurrentFontFlags = 0;
        gCurrentFontSpriteBase = FONT_SPRITE_BASE_MEDIUM;
        gfx_draw_string_centred(dpi, STR_COMMA16, x + 14, w->y + 32, COLOUR_WHITE | COLOUR_FLAG_OUTLINE, &i);
        x += 33;

    }
}