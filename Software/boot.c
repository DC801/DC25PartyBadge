/**
 *
 * @file boot.c
 *
 * @date May 24, 2017
 * @author hamster
 *
 * @brief This is the entry point for the DC801 DC25 party badge
 * 
 * This is the best place to start to figure out the program flow.
 * We setup the chip here, and then either jump to test mode or start the app
 *
 */

#include "system.h"
#include "boot.h"


// Timer counter
volatile uint16_t timerCounter;

/**
 * @brief Main entry app
 * @return Not used
 */
int main(void){

    // Configure the board
	// Timers
	app_timer_init();
	app_timer_create(&gameTimerID, APP_TIMER_MODE_REPEATED, timeout_handler);
	timerCounter = 0;

	// Bluetooth
	ble_stack_init();
    gap_params_init();

	// LEDs
    bsp_board_leds_init();

    // LCD - Reset first
    nrf_gpio_cfg_output(LCD_RESET);
    nrf_gpio_pin_clear(LCD_RESET);
    nrf_delay_ms(10);
    nrf_gpio_pin_set(LCD_RESET);
    nrf_gfx_init(p_lcd);

    // Speaker
    nrf_gpio_cfg_output(SPEAKER);

    // Buttons as inputs
    nrf_gpio_cfg_input(USER_BUTTON_1, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(USER_BUTTON_2, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(USER_BUTTON_3, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(USER_BUTTON_4, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(EAR_TOUCH, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(TAIL_TOUCH, NRF_GPIO_PIN_NOPULL);

    // Screen is upside down on the badge
    nrf_gfx_rotation_set(p_lcd, NRF_LCD_ROTATE_180);

    // Make the screen black to start with, so it's not white and annoying for long
    nrf_gfx_screen_fill(p_lcd, LCD_COLOR_BLACK);

    // Init the random number generator
    nrf_drv_rng_init(NULL);

    // Init the storage
    storage_init();

	// Setup the timer for the game, ticking once per second
	app_timer_start(gameTimerID, APP_TIMER_TICKS(1000), NULL);

    printf("Bootup\n");

    // Should we jump to the application or to test mode or reconfigure?
    if(isButtonDown(USER_BUTTON_4)){
    	// Button 4 was held down, so it's testing time
    	printf("Test Mode\n");
		run_test_mode();
	}
    else if(isButtonDown(USER_BUTTON_1)){
    	// Reconfigure selected
    	printf("App mode, reconfig\n");
		run_app_mode(true);
    }
    else{
    	printf("App mode\n");
    	run_app_mode(false);
    }

    // If you get here, you've exited the program, and the chip will halt

}

/**
 * @brief Called once a second, decrements the timerCounter for the game
 */
void timeout_handler(void * p_context){

    if(timerCounter > 0){
    	timerCounter--;
    }

}


