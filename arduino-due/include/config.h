#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// SCORING THRESHOLDS
// =============================================================================
#define BALL_DETECT_THRESHOLD 1000    
#define BALL_CLEAR_THRESHOLD  3700    

// =============================================================================
// PI CONTROLLER values
// =============================================================================

#define POSITION_MIN            0.0f
#define POSITION_MAX            5600.0f
#define POSITION_OVERSHOOT      5700

#define JOYSTICK_DEADZONE       5
#define JOYSTICK_GAIN           0.0015f
#define MOTOR_DEADZONE          0.1f

#define PI_KP                   0.050f
#define PI_KI                   0.002f
#define PI_SAMPLE_TIME          0.001f
#define PI_OUTPUT_MAX           100.0f
#define PI_OUTPUT_MIN           -100.0f


// =============================================================================
// SERVO MOTOR OFFSET
// =============================================================================

#define SERVO_CENTER 52
#define SERVO_DEADZONE 2
#define SERVO_OFFSET -2


#endif