/*
 * @file led.c
 *
 * @date May 25, 2017
 * @author hamster
 *
 * @brief Routines to handle drawing to groups of LEDs
 *
 */

#include "system.h"
#include "led.h"


/**
 * @brief Set the eye color
 * @param eye which eye to use, enum
 * @param color color to set, enum
 */
void set_eye_color(LED_EYE eye, EYE_COLOR color){

	// Left eye
	if(eye == eye_side_left || eye == eye_side_both){
		switch(color){
			case eye_color_none:
				nrf_gpio_pin_set(LED_EYE_LEFT_RED);
				nrf_gpio_pin_set(LED_EYE_LEFT_GREEN);
				nrf_gpio_pin_set(LED_EYE_LEFT_BLUE);
				break;
			case eye_color_blue:
				nrf_gpio_pin_set(LED_EYE_LEFT_RED);
				nrf_gpio_pin_set(LED_EYE_LEFT_GREEN);
				nrf_gpio_pin_clear(LED_EYE_LEFT_BLUE);
				break;
			case eye_color_green:
				nrf_gpio_pin_set(LED_EYE_LEFT_RED);
				nrf_gpio_pin_clear(LED_EYE_LEFT_GREEN);
				nrf_gpio_pin_set(LED_EYE_LEFT_BLUE);
				break;
			case eye_color_red:
				nrf_gpio_pin_clear(LED_EYE_LEFT_RED);
				nrf_gpio_pin_set(LED_EYE_LEFT_GREEN);
				nrf_gpio_pin_set(LED_EYE_LEFT_BLUE);
				break;
			case eye_color_all:
				nrf_gpio_pin_clear(LED_EYE_LEFT_RED);
				nrf_gpio_pin_clear(LED_EYE_LEFT_GREEN);
				nrf_gpio_pin_clear(LED_EYE_LEFT_BLUE);
				break;
			default:
				break;
		}
	}

	// Right eye
	if(eye == eye_side_right || eye == eye_side_both){
		switch(color){
			case eye_color_none:
				nrf_gpio_pin_set(LED_EYE_RIGHT_RED);
				nrf_gpio_pin_set(LED_EYE_RIGHT_GREEN);
				nrf_gpio_pin_set(LED_EYE_RIGHT_BLUE);
				break;
			case eye_color_blue:
				nrf_gpio_pin_set(LED_EYE_RIGHT_RED);
				nrf_gpio_pin_set(LED_EYE_RIGHT_GREEN);
				nrf_gpio_pin_clear(LED_EYE_RIGHT_BLUE);
				break;
			case eye_color_green:
				nrf_gpio_pin_set(LED_EYE_RIGHT_RED);
				nrf_gpio_pin_clear(LED_EYE_RIGHT_GREEN);
				nrf_gpio_pin_set(LED_EYE_RIGHT_BLUE);
				break;
			case eye_color_red:
				nrf_gpio_pin_clear(LED_EYE_RIGHT_RED);
				nrf_gpio_pin_set(LED_EYE_RIGHT_GREEN);
				nrf_gpio_pin_set(LED_EYE_RIGHT_BLUE);
				break;
			case eye_color_all:
				nrf_gpio_pin_clear(LED_EYE_RIGHT_RED);
				nrf_gpio_pin_clear(LED_EYE_RIGHT_GREEN);
				nrf_gpio_pin_clear(LED_EYE_RIGHT_BLUE);
				break;
			default:
				break;
		}
	}

}


/**
 * @brief Toggle all the helmet LEDs on or off
 * @param state true if you want the LEDs on
 */
void helmet_leds(uint8_t state){
	for (int i = 0; i < HELMET_LEDS_NUMBER; i++){
		nrf_gpio_pin_write(helmet_led_list[i], state);
	}
}

/**
 * @brief Toggle all the helmet wing LEDs on or off
 * @param state true if you want the LEDs on
 */
void wing_leds(uint8_t state){
	for (int i = 0; i < WING_LEDS_NUMBER; i++){
		nrf_gpio_pin_write(wing_led_list[i], state);
	}
}
