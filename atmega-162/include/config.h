/**
 * @file config.h
 * @brief System-wide configuration constants
 * 
 * All hardware-specific constants and calibration values.
 * This file should be the ONLY place with #defines for configuration.
 */

#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// SYSTEM CONFIGURATION
// =============================================================================

#define F_CPU 5000000UL

// =============================================================================
// HARDWARE MEMORY MAP
// =============================================================================

#define SRAM_BASE_ADDR  0x1400
#define ADC_BASE_ADDR   0x1000
#define SRAM_SIZE       0x0C00  // 3KB
#define ADC_SIZE        0x0400  // 1KB

// =============================================================================
// INPUT DEVICE CONFIGURATION
// =============================================================================

// LED count
#define LED_COUNT 6

// Joystick ADC calibration (determined manually)
#define JOY_X_MIN       68
#define JOY_X_MAX       248
#define JOY_Y_MIN       68
#define JOY_Y_MAX       248
#define JOY_X_CENTER    164
#define JOY_Y_CENTER    164

// Joystick deadzone
#define JOY_DEADZONE_X  20
#define JOY_DEADZONE_Y  20

#endif