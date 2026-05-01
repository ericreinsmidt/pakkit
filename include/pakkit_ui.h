/*
 * PakKit UI — Reusable UI components for NextUI paks
 * Header-only library. Built on top of Apostrophe.
 *
 * Usage:
 *   #define PAKKIT_UI_IMPLEMENTATION
 *   #include "pakkit_ui.h"
 *
 * Requires Apostrophe to be initialized before use.
 */

#ifndef PAKKIT_UI_H
#define PAKKIT_UI_H

#include "apostrophe.h"
#include "apostrophe_widgets.h"

/* -----------------------------------------------------------------------
 * Configuration
 * ----------------------------------------------------------------------- */

#ifndef PAKKIT_MAX_MENU_ITEMS
#define PAKKIT_MAX_MENU_ITEMS 12
#endif

#ifndef PAKKIT_SCROLL_STEP
#define PAKKIT_SCROLL_STEP 20
#endif

/* -----------------------------------------------------------------------
 * Hint bar (minimal footer replacement)
 * ----------------------------------------------------------------------- */

typedef struct {
    const char *button;
    const char *label;
} pakkit_hint;

void pakkit_draw_hints(pakkit_hint *hints, int count);

/* -----------------------------------------------------------------------
 * Message dialog
 * ----------------------------------------------------------------------- */

void pakkit_message(const char *message, const char *button_label);

/* -----------------------------------------------------------------------
 * Menu component
 * ----------------------------------------------------------------------- */

typedef struct {
    const char *label;
} pakkit_menu_item;

typedef struct {
    int selected_index;   /* -1 if cancelled */
} pakkit_menu_result;

int pakkit_menu(const char *title, pakkit_menu_item *items, int count,
                pakkit_menu_result *result);

/* -----------------------------------------------------------------------
 * List component
 * ----------------------------------------------------------------------- */

#ifndef PAKKIT_MAX_LIST_ITEMS
#define PAKKIT_MAX_LIST_ITEMS 64
#endif

#ifndef PAKKIT_LIST_VISIBLE
#define PAKKIT_LIST_VISIBLE 8
#endif

typedef enum {
    PAKKIT_ACTION_SELECTED,
    PAKKIT_ACTION_BACK,
    PAKKIT_ACTION_SECONDARY,
    PAKKIT_ACTION_TERTIARY,
} pakkit_action;

typedef struct {
    const char *label;
} pakkit_list_item;

typedef struct {
    const char      *title;
    pakkit_hint     *hints;
    int              hint_count;
    ap_button        secondary_button;   /* AP_BTN_NONE to disable */
    ap_button        tertiary_button;    /* AP_BTN_NONE to disable */
} pakkit_list_opts;

typedef struct {
    int            selected_index;   /* -1 if cancelled */
    pakkit_action  action;
} pakkit_list_result;

int pakkit_list(pakkit_list_opts *opts, pakkit_list_item *items, int count,
                pakkit_list_result *result);

/* -----------------------------------------------------------------------
 * Detail/About screen component
 * ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
 * Detail/About screen component
 * ----------------------------------------------------------------------- */

typedef struct {
    const char *key;
    const char *value;
} pakkit_info_pair;

typedef struct {
    const char       *title;
    const char       *subtitle;
    pakkit_info_pair *info;
    int               info_count;
    const char      **credits;
    int               credit_count;
} pakkit_detail_opts;

void pakkit_detail_screen(pakkit_detail_opts *opts);



/* -----------------------------------------------------------------------
 * Implementation
 * ----------------------------------------------------------------------- */

#ifdef PAKKIT_UI_IMPLEMENTATION

/* --- Hint bar --- */

void pakkit_draw_hints(pakkit_hint *hints, int count) {
    if (!hints || count <= 0) return;

    int sw = ap_get_screen_width();
    int sh = ap_get_screen_height();
    int pad = AP_DS(5);
    TTF_Font *font = ap_get_font(AP_FONT_TINY);
    ap_color color = ap_get_theme()->hint;
    int y = sh - TTF_FontHeight(font) - pad;

    /* Left-aligned hints */
    int x = pad * 2;
    for (int i = 0; i < count - 1; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s: %s", hints[i].button, hints[i].label);
        int w = ap_draw_text(font, buf, x, y, color);
        x += w + pad * 3;
    }

    /* Right-aligned last hint */
    if (count > 0) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s: %s",
                 hints[count - 1].button, hints[count - 1].label);
        int w = ap_measure_text(font, buf);
        ap_draw_text(font, buf, sw - w - pad * 2, y, color);
    }
}

/* --- Menu --- */

int pakkit_menu(const char *title, pakkit_menu_item *items, int count,
                pakkit_menu_result *result) {
    int cursor = 0;
    int running = 1;

    result->selected_index = -1;

    while (running) {
        ap_input_event ev;
        while (ap_poll_input(&ev)) {
            if (ev.pressed) {
                switch (ev.button) {
                    case AP_BTN_B:
                        if (!ev.repeated) {
                            result->selected_index = -1;
                            running = 0;
                        }
                        break;
                    case AP_BTN_A:
                        if (!ev.repeated) {
                            result->selected_index = cursor;
                            running = 0;
                        }
                        break;
                    case AP_BTN_UP:
                        cursor--;
                        if (cursor < 0) cursor = count - 1;
                        break;
                    case AP_BTN_DOWN:
                        cursor++;
                        if (cursor >= count) cursor = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        ap_clear_screen();
        ap_draw_background();

        int sw = ap_get_screen_width();
        int pad = AP_DS(5);

        TTF_Font *font_med   = ap_get_font(AP_FONT_MEDIUM);
        TTF_Font *font_small = ap_get_font(AP_FONT_SMALL);

        ap_theme *theme = ap_get_theme();
        ap_color text_color = theme->text;
        ap_color hint_color = theme->hint;
        ap_color highlight  = theme->highlight;
        ap_color hl_text    = theme->highlighted_text;

        /* Title */
        int y = pad * 3;
        ap_draw_text(font_med, title, pad * 3, y, hint_color);
        y += TTF_FontHeight(font_med) + pad * 3;

        /* Divider */
        ap_draw_rect(pad * 3, y, sw - pad * 6, 1, hint_color);
        y += pad * 3;

        /* Menu items */
        int item_h = TTF_FontHeight(font_small) + pad * 3;

        for (int i = 0; i < count; i++) {
            int pill_y = y + i * item_h;
            int text_y = pill_y + (item_h - TTF_FontHeight(font_small)) / 2;

            if (i == cursor) {
                ap_draw_pill(pad * 2, pill_y, sw - pad * 4, item_h, highlight);
                ap_draw_text(font_small, items[i].label, pad * 4, text_y, hl_text);
            } else {
                ap_draw_text(font_small, items[i].label, pad * 4, text_y, text_color);
            }
        }

        /* Hints */
        pakkit_hint hints[] = {
            {.button = "B",.label = "Back" },
            {.button = "A",.label = "Select" },
        };
        pakkit_draw_hints(hints, 2);

        ap_present();
    }

    return (result->selected_index >= 0) ? AP_OK : AP_CANCELLED;
}

/* --- List --- */

int pakkit_list(pakkit_list_opts *opts, pakkit_list_item *items, int count,
                pakkit_list_result *result) {
    int cursor = 0;
    int scroll = 0;
    int running = 1;

    result->selected_index = -1;
    result->action = PAKKIT_ACTION_BACK;

    while (running) {
        ap_input_event ev;
        while (ap_poll_input(&ev)) {
            if (ev.pressed) {
                switch (ev.button) {
                    case AP_BTN_B:
                        if (!ev.repeated) {
                            result->selected_index = -1;
                            result->action = PAKKIT_ACTION_BACK;
                            running = 0;
                        }
                        break;
                    case AP_BTN_A:
                        if (!ev.repeated) {
                            result->selected_index = cursor;
                            result->action = PAKKIT_ACTION_SELECTED;
                            running = 0;
                        }
                        break;
                    case AP_BTN_UP:
                        cursor--;
                        if (cursor < 0) cursor = count - 1;
                        break;
                    case AP_BTN_DOWN:
                        cursor++;
                        if (cursor >= count) cursor = 0;
                        break;
                    default:
                        /* Check secondary/tertiary */
                        if (!ev.repeated && opts->secondary_button != AP_BTN_NONE
                            && ev.button == opts->secondary_button) {
                            result->selected_index = cursor;
                            result->action = PAKKIT_ACTION_SECONDARY;
                            running = 0;
                        }
                        if (!ev.repeated && opts->tertiary_button != AP_BTN_NONE
                            && ev.button == opts->tertiary_button) {
                            result->selected_index = cursor;
                            result->action = PAKKIT_ACTION_TERTIARY;
                            running = 0;
                        }
                        break;
                }
            }
        }

        /* Keep cursor visible */
        int sw = ap_get_screen_width();
        int sh = ap_get_screen_height();
        int pad = AP_DS(5);

        TTF_Font *font_med   = ap_get_font(AP_FONT_MEDIUM);
        TTF_Font *font_small = ap_get_font(AP_FONT_SMALL);
        TTF_Font *font_tiny  = ap_get_font(AP_FONT_TINY);

        int item_h = TTF_FontHeight(font_small) + pad * 3;
        int title_h = TTF_FontHeight(font_med) + pad * 3 + 1 + pad * 3;
        int hint_h = TTF_FontHeight(font_tiny) + pad * 2;
        int list_area_h = sh - title_h - hint_h - pad;
        int visible = list_area_h / item_h;
        if (visible < 1) visible = 1;

        if (cursor < scroll) scroll = cursor;
        if (cursor >= scroll + visible) scroll = cursor - visible + 1;

        ap_clear_screen();
        ap_draw_background();

        ap_theme *theme = ap_get_theme();
        ap_color text_color = theme->text;
        ap_color hint_color = theme->hint;
        ap_color highlight  = theme->highlight;
        ap_color hl_text    = theme->highlighted_text;

        /* Title */
        int y = pad * 3;
        if (opts->title) {
            ap_draw_text(font_med, opts->title, pad * 3, y, hint_color);
            y += TTF_FontHeight(font_med) + pad * 3;
        }

        /* Divider */
        ap_draw_rect(pad * 3, y, sw - pad * 6, 1, hint_color);
        y += pad * 3;

        /* List items */
        int list_top = y;
        SDL_Rect clip = { 0, list_top, sw, list_area_h };
        SDL_RenderSetClipRect(ap__g.renderer, &clip);

        for (int i = scroll; i < count && i < scroll + visible; i++) {
            int item_y = list_top + (i - scroll) * item_h;

            int text_y = item_y + (item_h - TTF_FontHeight(font_small)) / 2;

            if (i == cursor) {
                ap_draw_pill(pad * 2, item_y, sw - pad * 4, item_h, highlight);
                ap_draw_text_ellipsized(font_small, items[i].label,
                                        pad * 4, text_y, hl_text, sw - pad * 8);
            } else {
                ap_draw_text_ellipsized(font_small, items[i].label,
                                        pad * 4, text_y, text_color, sw - pad * 8);
            }
        }

        SDL_RenderSetClipRect(ap__g.renderer, NULL);

        /* Scroll indicator */
        if (count > visible) {
            int bar_x = sw - pad * 2;
            int bar_h = list_area_h;
            int thumb_h = (visible * bar_h) / count;
            if (thumb_h < pad * 2) thumb_h = pad * 2;
            int thumb_y = list_top + (scroll * (bar_h - thumb_h)) / (count - visible);
            ap_color bar_color = { hint_color.r, hint_color.g, hint_color.b, 80 };
            ap_color thumb_color = { hint_color.r, hint_color.g, hint_color.b, 160 };
            ap_draw_rect(bar_x, list_top, 3, bar_h, bar_color);
            ap_draw_rect(bar_x, thumb_y, 3, thumb_h, thumb_color);
        }

        /* Hints */
        if (opts->hints && opts->hint_count > 0)
            pakkit_draw_hints(opts->hints, opts->hint_count);

        ap_present();
    }

    return (result->action == PAKKIT_ACTION_SELECTED) ? AP_OK : AP_CANCELLED;
}

/* --- Message dialog --- */

void pakkit_message(const char *message, const char *button_label) {
    if (!button_label) button_label = "OK";
    int running = 1;

    while (running) {
        ap_input_event ev;
        while (ap_poll_input(&ev)) {
            if (ev.pressed && !ev.repeated) {
                if (ev.button == AP_BTN_A || ev.button == AP_BTN_B)
                    running = 0;
            }
        }

        ap_clear_screen();
        ap_draw_background();

        int sw = ap_get_screen_width();
        int sh = ap_get_screen_height();
        int pad = AP_DS(5);

        TTF_Font *font_small = ap_get_font(AP_FONT_SMALL);
        TTF_Font *font_tiny  = ap_get_font(AP_FONT_TINY);

        ap_theme *theme = ap_get_theme();
        ap_color text_color = theme->text;
        ap_color hint_color = theme->hint;

        /* Center message vertically and horizontally */
        int max_w = sw - pad * 8;
        int msg_h = ap_measure_wrapped_text_height(font_small, message, max_w);
        int msg_y = (sh - msg_h) / 2;
        ap_draw_text_wrapped(font_small, message, pad * 4, msg_y, max_w,
                             text_color, AP_ALIGN_CENTER);

        /* Hint */
        char hint_buf[64];
        snprintf(hint_buf, sizeof(hint_buf), "A: %s", button_label);
        int hint_w = ap_measure_text(font_tiny, hint_buf);
        int hint_y = sh - TTF_FontHeight(font_tiny) - pad;
        ap_draw_text(font_tiny, hint_buf, (sw - hint_w) / 2, hint_y, hint_color);

        ap_present();
    }
}

/* --- Detail screen --- */
/* --- Message dialog --- */

void pakkit_message(const char *message, const char *button_label) {
    if (!button_label) button_label = "OK";
    int running = 1;

    while (running) {
        ap_input_event ev;
        while (ap_poll_input(&ev)) {
            if (ev.pressed && !ev.repeated) {
                if (ev.button == AP_BTN_A || ev.button == AP_BTN_B)
                    running = 0;
            }
        }

        ap_clear_screen();
        ap_draw_background();

        int sw = ap_get_screen_width();
        int sh = ap_get_screen_height();
        int pad = AP_DS(5);

        TTF_Font *font_small = ap_get_font(AP_FONT_SMALL);
        TTF_Font *font_tiny  = ap_get_font(AP_FONT_TINY);

        ap_theme *theme = ap_get_theme();
        ap_color text_color = theme->text;
        ap_color hint_color = theme->hint;

        /* Center message vertically and horizontally */
        int max_w = sw - pad * 8;
        int msg_h = ap_measure_wrapped_text_height(font_small, message, max_w);
        int msg_y = (sh - msg_h) / 2;
        ap_draw_text_wrapped(font_small, message, pad * 4, msg_y, max_w,
                             text_color, AP_ALIGN_CENTER);

        /* Hint */
        char hint_buf[64];
        snprintf(hint_buf, sizeof(hint_buf), "A: %s", button_label);
        int hint_w = ap_measure_text(font_tiny, hint_buf);
        int hint_y = sh - TTF_FontHeight(font_tiny) - pad;
        ap_draw_text(font_tiny, hint_buf, (sw - hint_w) / 2, hint_y, hint_color);

        ap_present();
    }
}

/* --- Detail screen --- */

void pakkit_detail_screen(pakkit_detail_opts *opts) {
    int running = 1;
    int scroll_y = 0;

    while (running) {
        ap_input_event ev;
        while (ap_poll_input(&ev)) {
            if (ev.pressed) {
                switch (ev.button) {
                    case AP_BTN_B:
                        if (!ev.repeated) running = 0;
                        break;
                    case AP_BTN_UP:
                        if (scroll_y > 0) scroll_y -= PAKKIT_SCROLL_STEP;
                        if (scroll_y < 0) scroll_y = 0;
                        break;
                    case AP_BTN_DOWN:
                        scroll_y += PAKKIT_SCROLL_STEP;
                        break;
                    default:
                        break;
                }
            }
        }

        ap_clear_screen();
        ap_draw_background();

        int sw = ap_get_screen_width();
        int sh = ap_get_screen_height();
        int pad = AP_DS(5);

        TTF_Font *font_large = ap_get_font(AP_FONT_LARGE);
        TTF_Font *font_small = ap_get_font(AP_FONT_SMALL);
        TTF_Font *font_tiny  = ap_get_font(AP_FONT_TINY);

        ap_theme *theme = ap_get_theme();
        ap_color text_color = theme->text;
        ap_color hint_color = theme->hint;

        int hint_font_h = TTF_FontHeight(font_tiny);
        int footer_h = hint_font_h + pad * 2;
        int content_top = pad;
        int content_bottom = sh - footer_h;
        int content_h = content_bottom - content_top;

        SDL_Rect clip = { 0, content_top, sw, content_h };
        SDL_RenderSetClipRect(ap__g.renderer, &clip);

        int y = content_top - scroll_y;

        /* Title */
        if (opts->title) {
            ap_draw_text(font_large, opts->title, pad * 3, y, text_color);
            y += TTF_FontHeight(font_large) + pad;
        }

        /* Subtitle */
        if (opts->subtitle) {
            ap_draw_text(font_small, opts->subtitle, pad * 3, y, hint_color);
            y += TTF_FontHeight(font_small) + pad * 3;
        } else {
            y += pad * 2;
        }

        /* Divider */
        ap_draw_rect(pad * 3, y, sw - pad * 6, 1, hint_color);
        y += pad * 3;

        /* Info pairs */
        if (opts->info && opts->info_count > 0) {
            int label_x = pad * 3;
            int value_x = pad * 3 + AP_DS(80);
            int row_h = TTF_FontHeight(font_small) + pad;

            for (int i = 0; i < opts->info_count; i++) {
                ap_draw_text(font_small, opts->info[i].key, label_x, y, hint_color);
                ap_draw_text(font_small, opts->info[i].value, value_x, y, text_color);
                y += row_h;
            }

            y += pad * 2;

            /* Divider */
            ap_draw_rect(pad * 3, y, sw - pad * 6, 1, hint_color);
            y += pad * 3;
        }

        /* Credits */
        if (opts->credits && opts->credit_count > 0) {
            int row_h = TTF_FontHeight(font_small) + pad;
            for (int i = 0; i < opts->credit_count; i++) {
                ap_color c = (i == 0) ? text_color : hint_color;
                ap_draw_text(font_small, opts->credits[i], pad * 3, y, c);
                y += row_h;
            }
            y += pad * 2;
        }

        /* Clamp scroll */
        int total_content = y + scroll_y - content_top;
        int max_scroll = total_content - content_h;
        if (max_scroll < 0) max_scroll = 0;
        if (scroll_y > max_scroll) scroll_y = max_scroll;

        SDL_RenderSetClipRect(ap__g.renderer, NULL);

        /* Hints */
        pakkit_hint hints[] = {
            {.button = "B",.label = "Back" },
        };
        pakkit_draw_hints(hints, 1);

        ap_present();
    }
}

#endif /* PAKKIT_UI_IMPLEMENTATION */
#endif /* PAKKIT_UI_H */
