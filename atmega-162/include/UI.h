#ifndef UI_H
#define UI_H

#include "types.h"

void draw_window(int X, int Y, unsigned char width_in_tiles, unsigned char height_in_tiles);
void joystick_indicator(char X, char Y, unsigned char hand);
void draw_printf(char x, char y, const char* fmt, ...);

// Menu system functions
void draw_menu(void);
void draw_menu_cursor(void);
void display_current_screen(void);

// Menu system
extern ScreenState current_screen;
extern CursorState cursor_state;


// Transition state
typedef enum {
    TRANSITION_NONE,
    TRANSITION_OUT,     // Fading out current screen
    TRANSITION_IN       // Fading in new screen
} TransitionState;

// Transition configuration
typedef struct {
    TransitionState state;
    uint8_t frame;              // Current frame of animation
    uint8_t total_frames;       // Total frames for transition (8 = smooth, 4 = fast)
    ScreenState target_screen;  // Screen to transition to
} ScreenTransition;

// Screen transition functions
void Transition_Init(void);
void Transition_Start(ScreenState new_screen);
void Transition_Update(void);
void Transition_Apply_Dither(uint8_t intensity);
uint8_t Transition_Is_Active(void);


#endif