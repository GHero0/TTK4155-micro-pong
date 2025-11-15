/**
 * @file menu.c
 * @brief MENU implementation - handles menu navigation and rendering only
 */

// =============================================================================
// INCLUDES
// =============================================================================

#include "menu.h"
#include "types.h"
#include "UI.h"
#include "joystick.h"
#include "drivers/IO_board.h"
#include "sprites.h"
#include "inter.h"
#include "drivers/CAN.h"

#include <stddef.h>
#include <avr/pgmspace.h>

// =============================================================================
// STATIC FUNCTION DECLARATIONS
// =============================================================================

static void go_back_to_parent(void);
static void action_debug_window(void);
static void action_placeholder_1(void);
static void action_placeholder_2(void);
static void action_debug_blue_box_joystick_map(void);
static void action_debug_blue_box_relative_map(void);
static void action_startup(void);
static void action_calibration_joystick(void);

static void enter_submenu(Menu *submenu);
static void menu_scroll_up(void);
static void menu_scroll_down(void);
static unsigned char is_cursor_over_item(unsigned char item_visual_index);
static void read_menu_item(const MenuItem *flash_item, MenuItem *ram_item);

// =============================================================================
// STATIC VARIABLES DECLARATIONS
// =============================================================================

static Menu submenu_1;
static Menu submenu_2;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

Menu *current_menu = NULL;
char menu_control_mode = 0;

// =============================================================================
// MENU DATA IN FLASH
// =============================================================================

static const MenuItem PROGMEM submenu_1_items[] = {
    {"..", go_back_to_parent, NULL, 31},
    {"Tilemap 1bpp", action_placeholder_1, NULL, 255},
    {"Tilemap 2bpp", action_placeholder_2, NULL, 255},
};

static const MenuItem PROGMEM submenu_2_items[] = {
    {"..", go_back_to_parent, NULL, 31},
    {"Mode Joystick Map", action_debug_blue_box_joystick_map, NULL, 255},
    {"Mode Relative Map", action_debug_blue_box_relative_map, NULL, 255},
};

static const MenuItem PROGMEM main_menu_items[] = {
    {"Joystick Cal.", action_calibration_joystick, NULL, 46},
    {"Debug IO-board", action_debug_window, NULL, 15},
    {"Debug Blue Box >", NULL, &submenu_2, 14},
    {"Tilemap >", NULL, &submenu_1, 91},
    {"SHUTDOWN", action_startup, NULL, 127}};

static Menu submenu_1 = {
    .selected_item = 0,
    .total_items = 3,
    .scroll_offset = 0,
    .items = submenu_1_items,
    .parent_menu = NULL,
};

static Menu submenu_2 = {
    .selected_item = 0,
    .total_items = 3,
    .scroll_offset = 0,
    .items = submenu_2_items,
    .parent_menu = NULL,
};

Menu main_menu = {
    .selected_item = 0,
    .total_items = 5,
    .scroll_offset = 0,
    .items = main_menu_items,
    .parent_menu = NULL,
};

// =============================================================================
// MENU HELPER FUNCTIONS
// =============================================================================

static unsigned char is_cursor_over_item(unsigned char item_visual_index)
{
    const signed char item_x = 8;
    const signed char item_y = 0 + (item_visual_index * 17);
    const signed char item_width = 114;
    const signed char item_height = 17;

    const signed char cx = cursor_state.x + 4;
    const signed char cy = cursor_state.y + 4;

    return (cx >= item_x && cx < item_x + item_width &&
            cy >= item_y && cy < item_y + item_height);
}

static void go_back_to_parent(void)
{
    if (current_menu != NULL && current_menu->parent_menu != NULL)
    {
        current_menu = current_menu->parent_menu;
    }
}

static void enter_submenu(Menu *submenu)
{
    if (submenu != NULL)
    {
        submenu->parent_menu = current_menu;
        submenu->selected_item = 0;
        submenu->scroll_offset = 0;
        current_menu = submenu;
    }
}

static void menu_scroll_up(void)
{
    if (current_menu == NULL)
        return;

    if (current_menu->selected_item > 0)
    {
        current_menu->selected_item--;
    }
    else if (current_menu->scroll_offset > 0)
    {
        current_menu->scroll_offset--;
    }
}

static void menu_scroll_down(void)
{
    if (current_menu == NULL)
        return;

    unsigned char absolute_selected = current_menu->scroll_offset + current_menu->selected_item;

    if (absolute_selected < current_menu->total_items - 1)
    {
        if (current_menu->selected_item < 4 - 1)
        {
            current_menu->selected_item++;
        }
        else
        {
            current_menu->scroll_offset++;
        }
    }
}

static void read_menu_item(const MenuItem *flash_item, MenuItem *ram_item)
{
    memcpy_P(ram_item, flash_item, sizeof(MenuItem));
}

// =============================================================================
// MENU ACTION CALLBACKS
// =============================================================================

static void action_debug_window(void)
{
    current_screen = SCREEN_DEBUG_IO_BOARD;
}

static void action_placeholder_1(void)
{
    current_screen = SCREEN_TILEMAP_1BPP;
}

static void action_placeholder_2(void)
{
    current_screen = SCREEN_TILEMAP_2BPP;
}

static void action_debug_blue_box_joystick_map(void)
{
    current_screen = SCREEN_DEBUG_BLUE_BOX;
    // Send joystick map
    msgCAN_TX.message_id = 7;
    msgCAN_TX.message_data_length = 1;
    msgCAN_TX.message_data[0] = 1; 
    CAN_Send_Message(msgCAN_TX);
}

static void action_debug_blue_box_relative_map(void)
{
    current_screen = SCREEN_DEBUG_BLUE_BOX;
    // Send relative map
    msgCAN_TX.message_id = 7;
    msgCAN_TX.message_data_length = 1;
    msgCAN_TX.message_data[0] = 2; 
    CAN_Send_Message(msgCAN_TX);
}


static void action_calibration_joystick(void)
{
    current_screen = SCREEN_CALIBRATION_JOYSTICK;
}

static void action_startup(void)
{
    current_screen = SCREEN_STARTUP;
}

// =============================================================================
// MENU RENDERING
// =============================================================================

void draw_menu(void)
{
    if (current_menu == NULL)
        return;

    // Input detection
    static struct
    {
        signed char x, y;
        unsigned char nav, R6, cursor_click, NU, ND, NB;
    } prev = {0};

    signed char old_x = cursor_state.x;
    signed char old_y = cursor_state.y;
    update_cursor_position();

    // Mode switching
    if (cursor_state.x != old_x || cursor_state.y != old_y)
    {
        if (cursor_state.x != prev.x || cursor_state.y != prev.y)
        {
            menu_control_mode = 0;
        }
    }
    prev.x = cursor_state.x;
    prev.y = cursor_state.y;

    unsigned char nav_pressed = buttons.nav & 0x1E;
    if (nav_pressed && !prev.nav)
        menu_control_mode = 1;
    prev.nav = nav_pressed;

    // Hover detection
    unsigned char cursor_hover = 0xFF;
    if (menu_control_mode == 0)
    {
        for (unsigned char i = 0; i < 4; i++)
        {
            if ((current_menu->scroll_offset + i) < current_menu->total_items)
            {
                if (is_cursor_over_item(i))
                {
                    cursor_hover = current_menu->selected_item = i;
                    break;
                }
            }
        }
    }

    // Draw highlight
    if (menu_control_mode == 1 || cursor_hover != 0xFF)
    {
        draw_rectangle(8, 1 + (current_menu->selected_item << 4), 112, 12);
    }

    // Draw items
    MenuItem temp_item;
    for (unsigned char i = 0; i < 4; i++)
    {
        unsigned char idx = current_menu->scroll_offset + i;
        if (idx < current_menu->total_items)
        {
            read_menu_item(&current_menu->items[idx], &temp_item);
            draw_menu_item(12, 3 + (i << 4), temp_item.label, temp_item.icon_tile);
        }
    }

    // Scrollbar
    draw_line(126, 0, 64, 1, 1);
    unsigned char bar_h = 5;
    unsigned char bar_y = (current_menu->total_items > 4) ? (current_menu->scroll_offset * (64 - bar_h)) / (current_menu->total_items - 4) : 0;
    for (unsigned char x = 125; x <= 127; x++)
    {
        draw_line(x, bar_y, bar_h, 1, 0);
    }

    // Input handling
    static unsigned char scroll_counter = 0;

    if (Flag_screen)
    {
        scroll_counter++;

        // Scrolling
        if (menu_control_mode == 1)
        {
            if (buttons.NU && !prev.NU)
                menu_scroll_up();
            if (buttons.ND && !prev.ND)
                menu_scroll_down();
            prev.NU = buttons.NU;
            prev.ND = buttons.ND;
        }
        else if (scroll_counter >= 5)
        {
            if (joystick_dir == DOWN)
                menu_scroll_down();
            else if (joystick_dir == UP)
                menu_scroll_up();
            scroll_counter = 0;
        }

        // Selection
        unsigned char select = (menu_control_mode == 1 && ((buttons.NB && !prev.NB) || (buttons.R6 && !prev.R6))) ||
                               (menu_control_mode == 0 && cursor_hover != 0xFF && cursor_state.clicking && !prev.cursor_click);

        if (select)
        {
            unsigned char abs_idx = current_menu->scroll_offset +
                                    (menu_control_mode ? current_menu->selected_item : cursor_hover);
            if (abs_idx < current_menu->total_items)
            {
                read_menu_item(&current_menu->items[abs_idx], &temp_item);
                if (temp_item.submenu)
                    enter_submenu(temp_item.submenu);
                else if (temp_item.action)
                    temp_item.action();
            }
        }

        prev.NB = buttons.NB;
        prev.R6 = buttons.R6;
        prev.cursor_click = cursor_state.clicking;

        // Back button
        if (buttons.R4 && !prev_R4 && current_menu->parent_menu)
        {
            go_back_to_parent();
        }
        prev_R4 = buttons.R4;
    }

    if (menu_control_mode == 0)
        draw_menu_cursor();
}