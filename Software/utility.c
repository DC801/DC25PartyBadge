/*
 * @file utility.c
 *
 * @date Jul 24, 2017
 * @author hamster
 *
 *  Utility functions
 *
 */

#include "system.h"
#include "utility.h"

/**
 * @brief Determine if a button is being pressed.
 * @return Button pressed, with long press mask if long pressed
 *
 * Buttons are debounced for a few milliseconds
 * Buttons held down for longer than the defined long press duration are sent
 * as a long press button
 */
int getButton(void){

	int longPress = 0;
	int button = 0;

	if(isButtonDown(USER_BUTTON_1)){
		// Debounce
		nrf_delay_ms(BUTTON_DEBOUNCE_MS);
		if(isButtonDown(USER_BUTTON_1)){
			button = USER_BUTTON_1;
			while(isButtonDown(USER_BUTTON_1)){
				nrf_delay_ms(1);
				longPress++;
				if(longPress > BUTTON_LONG_PRESS_MS){
					break;
				}
			}
		}
	}
	if(isButtonDown(USER_BUTTON_2)){
		// Debounce
		nrf_delay_ms(BUTTON_DEBOUNCE_MS);
		if(isButtonDown(USER_BUTTON_2)){
			button = USER_BUTTON_2;
			while(isButtonDown(USER_BUTTON_2)){
				nrf_delay_ms(1);
				longPress++;
				if(longPress > BUTTON_LONG_PRESS_MS){
					break;
				}
			}
		}
	}
	if(isButtonDown(USER_BUTTON_3)){
		// Debounce
		nrf_delay_ms(BUTTON_DEBOUNCE_MS);
		if(isButtonDown(USER_BUTTON_3)){
			button = USER_BUTTON_3;
			while(isButtonDown(USER_BUTTON_3)){
				nrf_delay_ms(1);
				longPress++;
				if(longPress > BUTTON_LONG_PRESS_MS){
					break;
				}
			}
		}
	}
	if(isButtonDown(USER_BUTTON_4)){
		// Debounce
		nrf_delay_ms(BUTTON_DEBOUNCE_MS);
		if(isButtonDown(USER_BUTTON_4)){
			button = USER_BUTTON_4;
			while(isButtonDown(USER_BUTTON_4)){
				nrf_delay_ms(1);
				longPress++;
				if(longPress > BUTTON_LONG_PRESS_MS){
					break;
				}
			}
		}
	}
	if(isButtonDown(TAIL_TOUCH)){
		// Debounce
		nrf_delay_ms(BUTTON_DEBOUNCE_MS);
		if(isButtonDown(TAIL_TOUCH)){
			button = TAIL_TOUCH;
			while(isButtonDown(TAIL_TOUCH)){
				nrf_delay_ms(1);
				longPress++;
				if(longPress > BUTTON_LONG_PRESS_MS){
					break;
				}
			}
		}
	}
	if(isButtonDown(EAR_TOUCH)){
		// Debounce
		nrf_delay_ms(BUTTON_DEBOUNCE_MS);
		if(isButtonDown(EAR_TOUCH)){
			button = EAR_TOUCH;
			while(isButtonDown(EAR_TOUCH)){
				nrf_delay_ms(1);
				longPress++;
				if(longPress > BUTTON_LONG_PRESS_MS){
					break;
				}
			}
		}
	}


	if(longPress > BUTTON_LONG_PRESS_MS){
		button |= LONG_PRESS_MASK;
	}

	return button;

}

/**
 * @brief Get a string from the user
 * @param string Where to place the chars
 * @param len Max length
 * @param point Start point to draw string
 * @param box Bounding box of chars to erase
 *
 * Draws the string as entered from the point passed in
 * Blinks the cursor every second
 */
void getString(char* string, int len, nrf_gfx_point_t *point, nrf_gfx_rect_t *box){

	int button;
	bool done = false;
	int pos = 0;
	int lastTick = 0;
	int changed = true;

	// Clear the string
	memset(string, 0, len);

	while(!done){

		// Display the current string
		if(changed){
			nrf_gfx_rect_draw(p_lcd, box, 0, LCD_COLOR_BLACK, true);
			nrf_gfx_print(p_lcd, point,
				LCD_COLOR_WHITE, string, font_8_point, true);
			changed = false;
		}

		button = getButton();

		switch(button){
			case USER_BUTTON_1:
			case USER_BUTTON_1_LONG:
				// +
				// If this entry is null, zip to the letters first
				if(string[pos] > LAST_ALLOWED_CHAR || string[pos] < FIRST_ALLOWED_CHAR){
					string[pos] = FIRST_LETTER - 1;
				}
				if(string[pos] < LAST_ALLOWED_CHAR && string[pos]){
					string[pos]++;
				}
				else{
					string[pos] = FIRST_ALLOWED_CHAR;
				}
				changed = true;
				break;
			case USER_BUTTON_2:
			case USER_BUTTON_2_LONG:
				// -
				// If this entry is null, zip to the letters first
				if(string[pos] > LAST_ALLOWED_CHAR || string[pos] < FIRST_ALLOWED_CHAR){
					string[pos] = LAST_LETTER + 1;
				}
				if(string[pos] > FIRST_ALLOWED_CHAR){
					string[pos]--;
				}
				else{
					string[pos] = LAST_ALLOWED_CHAR;
				}
				changed = true;
				break;
			case USER_BUTTON_3:
				if(pos < len - 1) pos++;
				// ENT
				break;
			case USER_BUTTON_4:
				if(pos > 0) pos--;
				// BACK
				break;
			case USER_BUTTON_3_LONG:
				// Done
				done = true;
				// Hold here until the user lets go
				printf("String: %s\n", string);
				nrf_gfx_rect_draw(p_lcd, box, 0, LCD_COLOR_BLACK, true);
				nrf_gfx_print(p_lcd, point,
					LCD_COLOR_WHITE, "SAVED", font_8_point, true);
				while(isButtonDown(USER_BUTTON_3));
				break;
			default:
				break;
		}
	}

}

/**
 * @brief Detects if a button is being held down
 * @param button Is this button pressed down?
 * @return true if button is pressed down
 */
bool isButtonDown(int button){

	if(nrf_gpio_pin_read(button) == BUTTON_PRESSED){
		return true;
	}

	return false;
}


/**
 * @brief Pause the program until a button has been pressed and released
 * @param button Wait on this button
 */
void pauseUntilPress(int button){

	while(true){
		if(nrf_gpio_pin_read(button) == BUTTON_PRESSED){
			// Debounce
			nrf_delay_ms(button);
			if(nrf_gpio_pin_read(button) == BUTTON_PRESSED){
				while(nrf_gpio_pin_read(button) == BUTTON_PRESSED);
				return;
			}
		}
	}
}


/**
 * @brief Beeps the speaker for a duration at a certain frequency
 * @param duration How long to beep
 * @param frequency Tone freq in hz
 *
 * @note Busy waits, frequency might not be exact, might sound uneven if the softdevice needs to do BLE things
 */
void beep(int duration, int frequency){

	// Figure out how many beeps
	float period = 1000 / (float)frequency;
	long counter = period * duration;
	float delay = period / 2;

	//printf("Duration: %d Freq: %d Period: %f Count: %ld Delay: %f\n", duration, frequency, period, counter, delay);

	for(long i = 0; i < counter; i++){
		nrf_gpio_pin_write(SPEAKER, 1);
		nrf_delay_us(delay * 1000);
		nrf_gpio_pin_write(SPEAKER, 0);
		nrf_delay_us(delay * 1000);
	}

}

/**
 * @brief Check if a string is blank
 * @param string String to check
 * @param len Number of chars to check
 * @return true if all chars are null
 */
bool stringIsBlank(char *string, int len){

	for(int i = 0; i < len; i++){
		// Is the char a null or a space char?
		if(!((string[i] == '\0') || (string[i] == ' '))){
			// Nope, the string has something in it
			return false;
		}
	}

	return true;
}



