/*
 * @file led.h
 *
 * @date May 25, 2017
 * @author hamster
 *
 */

#ifndef LED_H_
#define LED_H_

typedef enum {
	eye_side_neither,
	eye_side_left,
	eye_side_right,
	eye_side_both,
	NUM_LED_EYES
} LED_EYE;

typedef enum {
	eye_color_none,
	eye_color_red,
	eye_color_green,
	eye_color_blue,
	eye_color_all,
	NUM_EYE_COLORS
} EYE_COLOR;

// LEDs are on when the signal is low
#define ON 0
#define OFF 1

void set_eye_color(LED_EYE eye, EYE_COLOR color);
void helmet_leds(uint8_t state);
void wing_leds(uint8_t state);

#endif /* LED_H_ */
