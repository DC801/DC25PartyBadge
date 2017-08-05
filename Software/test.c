/*
 * @file test.c
 *
 * @date May 24, 2017
 * @author hamster
 *
 * @brief Test Routines
 *
 * This file contains routines to test all the parts of the badge
 * The tests are run if the right button is held down during power up.
 * The user is then presented with a menu to run the tests
 *
 */

#include "system.h"
#include "test.h"

/**
 * @brief Toggle all the helmet wing LEDs on and off
 */
void test_wing_leds(void){
	wing_leds(ON);
	nrf_delay_ms(500);
	wing_leds(OFF);
	nrf_delay_ms(500);
	wing_leds(ON);
	nrf_delay_ms(500);
	wing_leds(OFF);
	nrf_delay_ms(500);
}

/**
 * @brief Toggle all the helmet LEDs on and off
 */
void test_helmet_leds(void){
	helmet_leds(ON);
	nrf_delay_ms(500);
	helmet_leds(OFF);
	nrf_delay_ms(500);
	helmet_leds(ON);
	nrf_delay_ms(500);
	helmet_leds(OFF);
	nrf_delay_ms(500);
}

/**
 * @brief Test the eyes by running through all the defined eye colors
 */
void test_eye_leds(void){
	for(int i = 0; i < NUM_EYE_COLORS; i++){
		set_eye_color(eye_side_both, i);
		nrf_delay_ms(500);
	}
	set_eye_color(eye_side_both, eye_color_none);
	nrf_delay_ms(500);
}


/**
 * @brief Fill the screen with solid colors
 */
void test_lcd(void){
	for(int j = 0; j < LCD_COLOR_NUMBER; j++){
		nrf_gfx_screen_fill(p_lcd, lcd_color_list[j]);
		nrf_delay_ms(100);
	}
	nrf_delay_ms(500);
}

/**
 * @brief Play a tone through the speaker
 */
void test_speaker(void){

	for(int i = 0; i < 50; i++){
		beep(25, 600 + (i * 10));
	}

}

/**
 * @brief Test the buttons
 */
void test_buttons(void){

	uint8_t keep_testing = true;
	char *new_button = "   ", *old_button = "   ";

	// Clear the screen
	nrf_gfx_screen_fill(p_lcd, LCD_COLOR_BLACK);

    nrf_gfx_point_t banner_start = NRF_GFX_POINT(20, 0);
    nrf_gfx_print(p_lcd, &banner_start, LCD_COLOR_WHITE, "DC801", font_16_point, true);

    nrf_gfx_point_t banner_2 = NRF_GFX_POINT(15, 25);
    nrf_gfx_print(p_lcd, &banner_2, LCD_COLOR_WHITE, "Press a button", font_8_point, true);
    nrf_gfx_point_t banner_3 = NRF_GFX_POINT(15, 40);
    nrf_gfx_print(p_lcd, &banner_3, LCD_COLOR_WHITE, "1+4 to exit", font_8_point, true);

    nrf_gfx_point_t banner_4 = NRF_GFX_POINT(15, 65);
    nrf_gfx_rect_t box = NRF_GFX_RECT(0, 65, 128, 25);

    while(keep_testing){

    	if(isButtonDown(USER_BUTTON_1) &&
    			isButtonDown(USER_BUTTON_4)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_1) &&
    				isButtonDown(USER_BUTTON_4)){
    			// Test is over
    			keep_testing = false;
    		}
    	}

    	if(isButtonDown(USER_BUTTON_1)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_1)){
    			new_button = "B1";
    		}
    	}
    	if(isButtonDown(USER_BUTTON_2)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_2)){
    			new_button = "B2";
    		}
    	}
    	if(isButtonDown(USER_BUTTON_3)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_3)){
    			new_button = "B3";
    		}
    	}
    	if(isButtonDown(USER_BUTTON_4)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_4)){
    			new_button = "B4";
    		}
    	}
    	if(isButtonDown(TAIL_TOUCH)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(TAIL_TOUCH)){
    			new_button = "TAIL";
    		}
    	}
    	if(isButtonDown(EAR_TOUCH)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(EAR_TOUCH)){
    			new_button = "EAR";
    		}
    	}

    	if(new_button != old_button){
    		nrf_gfx_rect_draw(p_lcd, &box, 1, LCD_COLOR_BLACK, true);
    		nrf_gfx_print(p_lcd, &banner_4, LCD_COLOR_RED, new_button, font_16_point, true);
    		old_button = new_button;
    	}

    	nrf_delay_ms(10);

    }


	nrf_delay_ms(500);
}

/**
 * @brief This is the test mode, for testing parts of the badge
 * @note You must reboot the badge to exit - button 4 will do this
 */
void run_test_mode(void){

    uint8_t test_mode = test_mode_none;
    uint8_t run_mode = run_mode_none;

    nrf_gfx_point_t status_line = NRF_GFX_POINT(5, 100);
    nrf_gfx_rect_t box = NRF_GFX_RECT(0, 100, 128, 14);

    draw_menu();

    while(isButtonDown(USER_BUTTON_4)){
    	// Wait for the user to let go of button 4 so we don't go right to button 4 test
    	nrf_delay_ms(10);
    }

    while (true){

    	// Scan for button changes
    	if(isButtonDown(USER_BUTTON_1)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_1)){
    			// Button is pressed, advance to the next test
				if(test_mode < test_mode_lcd){
					// Skip the button test
					test_mode++;
				}
				else{
					// All tests have been run
					test_mode = test_mode_eyes;
				}
    			run_mode = run_mode_single;
    		}
    	}

    	if(isButtonDown(USER_BUTTON_2)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_2)){
    			// Button is pressed, run all the tests
    			test_mode = test_mode_eyes;
    			run_mode = run_mode_all;
    		}
    	}

    	if(isButtonDown(USER_BUTTON_3)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_3)){
    			// Button is pressed, run the button test
    			test_mode = test_mode_button;
    			run_mode = run_mode_single;
    		}
    	}

    	if(isButtonDown(USER_BUTTON_4)){
    		// Debounce
    		nrf_delay_ms(15);
    		if(isButtonDown(USER_BUTTON_4)){
    			// Button is pressed, reboot
    			NVIC_SystemReset();
    		}
    	}

    	if(run_mode == run_mode_single || run_mode == run_mode_all){
			switch(test_mode){
				case test_mode_none:
				default:
					break;
				case test_mode_eyes:
					nrf_gfx_print(p_lcd, &status_line, LCD_COLOR_RED, "Eye LED Test", font_8_point, true);
					test_eye_leds();
					break;
				case test_mode_helmet:
					nrf_gfx_print(p_lcd, &status_line, LCD_COLOR_RED, "Helmet LED Test", font_8_point, true);
					test_helmet_leds();
					break;
				case test_mode_wingss:
					nrf_gfx_print(p_lcd, &status_line, LCD_COLOR_RED, "Wing LED Test", font_8_point, true);
					test_wing_leds();
					break;
				case test_mode_speaker:
					nrf_gfx_print(p_lcd, &status_line, LCD_COLOR_RED, "Speaker Test", font_8_point, true);
					test_speaker();
					break;
				case test_mode_lcd:
					nrf_gfx_print(p_lcd, &status_line, LCD_COLOR_RED, "LCD Test", font_8_point, true);
					test_lcd();
					break;
				case test_mode_button:
					nrf_gfx_print(p_lcd, &status_line, LCD_COLOR_RED, "Button Test", font_8_point, true);
					test_buttons();
					break;
			}

			if(run_mode == run_mode_all){
				// Advance to the next test
				if(test_mode < test_mode_lcd){
					// Skip the button test
					test_mode++;
					nrf_gfx_rect_draw(p_lcd, &box, 1, LCD_COLOR_BLACK, true);
				}
				else{
					// All tests have been run
					run_mode = run_mode_none;
					draw_menu();
				}
			}
			else{
				// Single test mode, cancel running again
				run_mode = run_mode_none;
				draw_menu();
			}

    	}

    	nrf_delay_ms(10);

    }

}

/**
 * @brief Draw the test menu.  This also clears any text at the bottom
 */
void draw_menu(void){

	nrf_gfx_screen_fill(p_lcd, LCD_COLOR_BLACK);

    // Print the test menu
    nrf_gfx_point_t banner_start = NRF_GFX_POINT(20, 0);
    nrf_gfx_print(p_lcd, &banner_start, LCD_COLOR_WHITE, "DC801", font_16_point, true);

    nrf_gfx_point_t banner_2 = NRF_GFX_POINT(28, 25);
    nrf_gfx_print(p_lcd, &banner_2, LCD_COLOR_BLUE, "Test Mode", font_8_point, true);

    nrf_gfx_point_t menu_line_1 = NRF_GFX_POINT(15, 40);
    nrf_gfx_print(p_lcd, &menu_line_1, LCD_COLOR_WHITE, "B1: Next Test", font_8_point, true);
    nrf_gfx_point_t menu_line_2 = NRF_GFX_POINT(15, 54);
    nrf_gfx_print(p_lcd, &menu_line_2, LCD_COLOR_WHITE, "B2: All Tests", font_8_point, true);
    nrf_gfx_point_t menu_line_3 = NRF_GFX_POINT(15, 68);
    nrf_gfx_print(p_lcd, &menu_line_3, LCD_COLOR_WHITE, "B3: Button Test", font_8_point, true);
    nrf_gfx_point_t menu_line_4 = NRF_GFX_POINT(15, 82);
    nrf_gfx_print(p_lcd, &menu_line_4, LCD_COLOR_WHITE, "B4: Reboot", font_8_point, true);

    nrf_gfx_rect_t box = NRF_GFX_RECT(0, 100, 14, 128);
    nrf_gfx_rect_draw(p_lcd, &box, 0, LCD_COLOR_BLACK, true);


}
