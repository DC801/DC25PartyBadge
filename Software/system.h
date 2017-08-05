/*
 * @file system.h
 *
 * @date May 24, 2017
 * @author hamster
 *
 * Common header file included by all files in the project
 * This will pull in all the other header files so our includes are neater
 *
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

// Our board file is custom_board.h
// The makefile defines BOARD_CUSTOM which will cause boards.h to include it
#include "boards.h"
// .. but we include the custom_board.h file anyway so that Eclipse can resolve
// the defines
#include "custom_board.h"

// Nordic headers
#include "nrf_gfx.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "bsp.h"
#include "app_timer.h"
#include "app_uart.h"
#include "nrf_drv_rng.h"
#include "nrf_drv_clock.h"
#include "SEGGER_RTT.h"


// System headers
#include <stdbool.h>
#include <stdint.h>

// Routines for testing the board
#include "test.h"

// Boot stuff
#include "boot.h"

// Main application
#include "app.h"

// LCD functions
#include "lcd.h"

// LED functions
#include "led.h"

// FDS storage functions
#include "storage.h"

// BLE functions
#include "ble.h"

// Utility functions
#include "utility.h"


#endif /* SYSTEM_H_ */
