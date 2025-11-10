/**
 * @file types.h
 * @brief Common type definitions used throughout the project
 * 
 * All structs, enums, and typedefs in one place.
 * No extern declarations or function prototypes here.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// JOYSTICK TYPES
// =============================================================================

/**
 * @brief Joystick position in Q8.8 fixed-point format
 * 
 * Range: -100.00 to +100.00 (stored as -25600 to +25600)
 * To convert to float: value / 256.0
 */
typedef struct
{
    int16_t X;  // Q8.8: –100.00 … +100.00
    int16_t Y;  // Q8.8: –100.00 … +100.00
} JoyPos;

/**
 * @brief Joystick direction enumeration
 */
typedef enum
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NEUTRAL,
} JoyDir;

// =============================================================================
// IO BOARD TYPES
// =============================================================================

/**
 * @brief Button states with bit-field access
 * 
 * Packed structure for efficient storage and flexible access.
 * Can access as whole bytes or individual bits.
 */
typedef struct __attribute__((packed))
{
    union
    {
        uint8_t right;
        struct
        {
            uint8_t R1 : 1;
            uint8_t R2 : 1;
            uint8_t R3 : 1;
            uint8_t R4 : 1;
            uint8_t R5 : 1;
            uint8_t R6 : 1;
        };
    };
    union
    {
        uint8_t left;
        struct
        {
            uint8_t L1 : 1;
            uint8_t L2 : 1;
            uint8_t L3 : 1;
            uint8_t L4 : 1;
            uint8_t L5 : 1;
            uint8_t L6 : 1;
            uint8_t L7 : 1;
        };
    };
    union
    {
        uint8_t nav;
        struct
        {
            uint8_t NB : 1;
            uint8_t NR : 1;
            uint8_t ND : 1;
            uint8_t NL : 1;
            uint8_t NU : 1;
        };
    };
} Buttons;

/**
 * @brief LED state (on/off + PWM brightness)
 */
typedef struct {
    unsigned char pwm;
    bool on;
} LedState;

/**
 * @brief Touchpad position and pressure
 */
typedef struct {
    unsigned char x;
    unsigned char y;
    unsigned char size;
} TouchPad;

// =============================================================================
// GRAPHICS TYPES
// =============================================================================

/**
 * @brief Sprite definition with tile indices and transformations
 * 
 * Sprites are composed of tiles from the tilemap.
 * trans_tiles defines transformations for each tile.
 */
typedef struct
{
    char height_tile;
    char width_tile;
    const unsigned char *sprite_tiles;
    const unsigned char *trans_tiles;
} Sprite;

// =============================================================================
// CAN TYPES
// =============================================================================

/**
 * @brief CAN message structure
 */
typedef struct {
	uint16_t message_id;
	char message_data_length;
	char message_data[8];
} CANMessage;

// =============================================================================
// UI/MENU TYPES
// =============================================================================

/**
 * @brief Screen state 
 */
typedef enum {
    SCREEN_MENU,
    SCREEN_DEBUG_IO_BOARD,
    SCREEN_DEBUG_BLUE_BOX,
    SCREEN_CALIBRATION_JOYSTICK
} ScreenState;

/**
 * @brief Cursor position and state
 */
typedef struct {
    signed char x;
    signed char y;
    unsigned char clicking;
} CursorState;

typedef struct Menu Menu;
typedef void (*MenuAction)(void);

/**
 * @brief Menu item struct with action handler
 */
typedef struct MenuItem {
    const char* label;
    MenuAction action;
    Menu* submenu;
    unsigned char icon_tile;
} MenuItem;

/**
 * @brief Menu structure
 */
struct Menu {
    unsigned char selected_item;
    unsigned char total_items;
    unsigned char scroll_offset;
    const MenuItem* items;
    Menu* parent_menu;
};

#endif 