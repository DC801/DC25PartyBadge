/**
 *
 * @file custom_board.h
 *
 * @date May 24, 2017
 * @author hamster
 *
 * This is the custom board defines for the LEDs and etc on the DC25 badge
 *
 */

#ifndef DC801_H
#define DC801_H

#ifdef __cplusplus
extern "C" {
#endif

// LEDs definitions

// Low is active
#define LEDS_ACTIVE_STATE 0

// LED Pins

// Helmet Wings
// Right, from center
#define LED_WING_RIGHT_RED_1     1
#define LED_WING_RIGHT_RED_2     2
#define LED_WING_RIGHT_RED_3     6
// Left, from center
#define LED_WING_LEFT_RED_1      7
#define LED_WING_LEFT_RED_2      8
#define LED_WING_LEFT_RED_3      9

// Helmet
#define LED_HELMET_YELLOW_1		25
#define LED_HELMET_YELLOW_2		26
#define LED_HELMET_YELLOW_3		27
#define LED_HELMET_YELLOW_4		28
#define LED_HELMET_YELLOW_5		30
#define LED_HELMET_YELLOW_6		31
#define LED_HELMET_YELLOW_7		0

// Eyes
// Right
#define LED_EYE_RIGHT_RED		15
#define LED_EYE_RIGHT_GREEN		14
//#define LED_EYE_RIGHT_BLUE		12 // only on proto boards
#define LED_EYE_RIGHT_BLUE		13
// Left
#define LED_EYE_LEFT_RED		16
#define LED_EYE_LEFT_GREEN		17
#define LED_EYE_LEFT_BLUE		18

// Arrays to make iteration easy
// The Nordic SDK uses LEDS_LIST internally
#define LEDS_LIST { \
	LED_WING_RIGHT_RED_1, 	\
	LED_WING_RIGHT_RED_2, 	\
	LED_WING_RIGHT_RED_3,	\
	LED_WING_LEFT_RED_1,	\
	LED_WING_LEFT_RED_2,	\
	LED_WING_LEFT_RED_3,	\
	LED_HELMET_YELLOW_1,	\
	LED_HELMET_YELLOW_2,	\
	LED_HELMET_YELLOW_3,	\
	LED_HELMET_YELLOW_4,	\
	LED_HELMET_YELLOW_5,	\
	LED_HELMET_YELLOW_6,	\
	LED_HELMET_YELLOW_7,	\
	LED_EYE_RIGHT_RED,		\
	LED_EYE_RIGHT_GREEN,	\
	LED_EYE_RIGHT_BLUE,		\
	LED_EYE_LEFT_RED,		\
	LED_EYE_LEFT_GREEN,		\
	LED_EYE_LEFT_BLUE	}

#define LEDS_NUMBER    		19

// LEDs in the helmet
#define HELMET_LEDS { \
		LED_HELMET_YELLOW_1,	\
		LED_HELMET_YELLOW_2,	\
		LED_HELMET_YELLOW_3,	\
		LED_HELMET_YELLOW_4,	\
		LED_HELMET_YELLOW_5,	\
		LED_HELMET_YELLOW_6,	\
		LED_HELMET_YELLOW_7	}

#define HELMET_LEDS_NUMBER 7

static const uint32_t helmet_led_list[HELMET_LEDS_NUMBER] = HELMET_LEDS;

// LEDs in the helmet wings
#define WING_LEDS { \
		LED_WING_RIGHT_RED_1, 	\
		LED_WING_RIGHT_RED_2, 	\
		LED_WING_RIGHT_RED_3,	\
		LED_WING_LEFT_RED_1,	\
		LED_WING_LEFT_RED_2,	\
		LED_WING_LEFT_RED_3 }

#define WING_LEDS_NUMBER 6

static const uint32_t wing_led_list[WING_LEDS_NUMBER] = WING_LEDS;

// Push buttons

#define BUTTONS_NUMBER 6

#define USER_BUTTON_1       3
#define USER_BUTTON_2       4
#define USER_BUTTON_3       10
#define USER_BUTTON_4       11
#define TAIL_TOUCH			5
#define EAR_TOUCH			29

#define LONG_PRESS_MASK		0x8000

#define USER_BUTTON_1_LONG       (3  | LONG_PRESS_MASK)
#define USER_BUTTON_2_LONG       (4  | LONG_PRESS_MASK)
#define USER_BUTTON_3_LONG       (10 | LONG_PRESS_MASK)
#define USER_BUTTON_4_LONG       (11 | LONG_PRESS_MASK)
#define TAIL_TOUCH_LONG			 (5  | LONG_PRESS_MASK)
#define EAR_TOUCH_LONG			 (29 | LONG_PRESS_MASK)

#define BUTTONS_LIST { \
	USER_BUTTON_1,	\
	USER_BUTTON_2,	\
	USER_BUTTON_3,	\
	USER_BUTTON_4,	\
	TAIL_TOUCH,		\
	EAR_TOUCH	}

// Low is active
#define BUTTONS_ACTIVE_STATE	0
// Don't need a pull up
#define BUTTON_PULL				0

// Just one speaker
#define SPEAKER		19

// LCD
// The SPI ports are defined in the config/sdk_config.h
#define LCD_RESET	23

// Clock
// Low frequency clock source to be used by the SoftDevice
// We're using the internal RC osc
#define NRF_CLOCK_LFCLKSRC      {.source        = NRF_CLOCK_LF_SRC_RC, \
                                 .rc_ctiv       = 1, \
                                 .rc_temp_ctiv  = 2, \
                                 .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM}


#ifdef __cplusplus
}
#endif

#endif // DC801_H
