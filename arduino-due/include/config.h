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

#define JOYSTICK_DEADZONE       1
#define JOYSTICK_GAIN           0.0015f
#define MOTOR_DEADZONE          0.1f

#define PI_KP                   0.080f
#define PI_KI                   0.003f
#define PI_SAMPLE_TIME          0.001f
#define PI_OUTPUT_MAX           50.0f
#define PI_OUTPUT_MIN           -50.0f


// =============================================================================
// SERVO MOTOR OFFSET
// =============================================================================

#define SERVO_CENTER 52
#define SERVO_DEADZONE 2
#define SERVO_OFFSET -2


#endif