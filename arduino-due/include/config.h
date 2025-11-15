#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// SCORING THRESHOLDS
// =============================================================================

#define BALL_DETECT_THRESHOLD 2000    
#define BALL_CLEAR_THRESHOLD  3000    

#define ADC_AVG_COUNT 100

// =============================================================================
// PI CONTROLLER values
// =============================================================================

#define POSITION_MIN            0.0f
#define POSITION_MAX            5600.0f
#define POSITION_OVERSHOOT      5700

#define JOYSTICK_DEADZONE       0
#define JOYSTICK_GAIN           1.3f
#define MOTOR_DEADZONE          0.1f

#define PI_KP                   0.050f
#define PI_KI                   0.0002f
#define PI_SAMPLE_TIME          0.050f 
#define PI_OUTPUT_MAX           100.0f
#define PI_OUTPUT_MIN           -100.0f


// =============================================================================
// SERVO MOTOR OFFSET
// =============================================================================

#define SERVO_CENTER 55
#define SERVO_DEADZONE 5
#define SERVO_OFFSET 8


#endif