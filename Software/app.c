/*
 *  @file app.c
 *  @date May 24, 2017
 *  @author hamster
 *  @brief Main app
 *
 *  Basically, we are going to show a series of images, scan for other badges, play the game, and configure the badge.
 *  Simple enough, right?
 *
 *  @note printf uses the Real Time Terminal function of the J-Link to print debug messages, see utility.h
 *
 */

#include "system.h"
#include "app.h"

// Images
// Images start as a bmp.  A make script compiles a converter and converts them
// to a C header file.  Each image uses up about 24k of space.

// Not enough flash space to fit them all
//#include "test_colors.h"
//#include "dc503.h"
//#include "game_start.h"
#include "hack_west.h"
#include "DC801UTwhite16.h"
#include "bender.h"
#include "crypto_village.h"
#include "queercon.h"
#include "dczia.h"
#include "shotbot.h"
#include "party.h"
// Images shown for the game
#include "game_win.h"
#include "game_lost.h"

// Various points on the screen
// Nordic's SDK for graphics requires you define a box or point to draw to
nrf_gfx_point_t top_line = NRF_GFX_POINT(5, 2);
nrf_gfx_point_t bottom_line = NRF_GFX_POINT(5, 116);
nrf_gfx_point_t top_line_centered = NRF_GFX_POINT(33, 2);
nrf_gfx_point_t bottom_line_centered = NRF_GFX_POINT(35, 116);

nrf_gfx_point_t entry_line = NRF_GFX_POINT(20, 80);
nrf_gfx_rect_t entry_box =
	{
		.x = 5,
		.y = 80,
		.width = 128,
		.height = 14
	};

nrf_gfx_rect_t top_line_box =
	{
		.x = 0,
		.y = 0,
		.width = 128,
		.height = 16
	};

nrf_gfx_rect_t bottom_line_box =
	{
		.x = 0,
		.y = 117,
		.width = 128,
		.height = 16
	};


nrf_gfx_rect_t middle_image_box =
	{
		.x = 0,
		.y = 17,
		.width = 128,
		.height = 100
	};

nrf_gfx_rect_t full_screen_image_box =
	{
		.x = 0,
		.y = 0,
		.width = 128,
		.height = 128
	};

// Struct of types of badges we look for
SEEN_MANUFACTURER seen_manufacturer;

// Struct of data for the infection game
volatile INFECTION_PARAMS infectionParams;

// User and vector info
CONFIG_PARAMS configParams;

// Game data
GAME_DATA gameData;

// Command data
COMMAND_DATA commandData;

// Are we sending a command?
bool sendingCommand = false;


/**
 * @brief Test function to display each image for a second to check how they look
 * @note Uncomment and call to use
 */
//void loopImages(void){
//
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, hack_west_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, DC801UTwhite16_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, bender_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, crypto_village_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, dc503_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, queercon_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, dczia_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, party_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, game_win_bmp);
//	nrf_delay_ms(1000);
//	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, game_lost_bmp);
//	nrf_delay_ms(1000);
//
//}

/**
 * @brief This is the main application
 * @param setup Should we go through setup again?
 *
 * We begin by checking the current config stored to flash
 * Then we read that in and setup the badge
 * Finally, we sit in a loop and display the images
 *
 * Each time we go through the loop, we expire all seen badges and commands
 * The first image is the DC801 logo, shown for 2 seconds to start seeing other BLE broadcasts
 * Then we check if it is time to play the game
 * Next we display the images for currently seen badges
 *
 */
void run_app_mode(bool setup){

	uint32_t data;
	uint8_t rand = 0;

	// Zero out the strings
	memset(configParams.username, 0, sizeof(configParams.username));
	memset(configParams.vector, 0, sizeof(configParams.vector));

	// Init all data stores
	clearSeenManufacturer();
	clearGameData();
	clearCommandData();

	// Pop pop!
	beep(75, 600);
	nrf_delay_ms(50);
	beep(150, 800);
	nrf_delay_ms(50);

	// Show every boot
	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, hack_west_bmp);
	nrf_delay_ms(100);
	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, DC801UTwhite16_bmp);
	nrf_delay_ms(100);
	nrf_gfx_screen_fill(p_lcd, LCD_COLOR_BLACK);

	// Initial dice roll and generate a wait time
	nrf_drv_rng_rand(&rand, 1);
	infectionParams.diceRoll = rand;

	nrf_drv_rng_rand(&rand, 1);
	infectionParams.waitTime = rand + MIN_WAIT_TIME;

	// Are we configured?
	if(storage_read_int(FILE_CONFIGURATION, RECORD_CONFIGURED, &data) != FDS_SUCCESS){
		// Not configured. Jump to the first time setup
		printf("Not configured\n");
		badgeSetup();
	}

	// Do we need to re-configure?
	if(setup){
		printf("Need re-config\n");
		badgeSetup();
	}

	// Get the username
	if(storage_read_string(FILE_CONFIGURATION, RECORD_USERNAME, configParams.username, 8) != FDS_SUCCESS){
		// Username is not stored?  Better reconfig.
		printf("Username not stored\n");
		badgeSetup();
	}
	// Get the vector
	if(storage_read_string(FILE_CONFIGURATION, RECORD_VECTOR, configParams.vector, 8) != FDS_SUCCESS){
		// Vector is not stored?  Better reconfig.
		printf("Vector not stored\n");
		badgeSetup();
	}

	printf("Apply stored config\n");

	// Format the data
	// If the strings are empty or just contain spaces, they are considered blank
	if(stringIsBlank(configParams.username, 8)){
		// Select a default
		printf("Username is blank\n");
		memcpy(configParams.username, DEFAULT_USER, 8);
		storage_delete(FILE_CONFIGURATION, RECORD_USERNAME);
		storage_write(FILE_CONFIGURATION, RECORD_USERNAME, configParams.username, 8);
	}

	if(stringIsBlank(configParams.vector, 8)){
		// Select a default, and save it to the config.
		// You had one chance to configure this, man, and you skipped it?
		// Welcome to Offensive Vector team _Helga_.
		printf("Vector is blank\n");
		memcpy(configParams.vector, DEFAULT_VECTOR, 8);
		storage_delete(FILE_CONFIGURATION, RECORD_VECTOR);
		storage_write(FILE_CONFIGURATION, RECORD_VECTOR, configParams.vector, 8);
	}

	printf("User: %s\n", configParams.username);
	printf("Vector: %s\n", configParams.vector);
	printf("Wait time: %d\n", infectionParams.waitTime);
	printf("Applying config\n");

	// We're configured.

	// Show the username on the screen
	nrf_gfx_screen_fill(p_lcd, LCD_COLOR_BLACK);
    nrf_gfx_print(p_lcd, &top_line_centered, LCD_COLOR_WHITE, configParams.username, font_8_point, true);
    nrf_gfx_print(p_lcd, &top_line, LCD_COLOR_WHITE, "  @", font_8_point, true);
    nrf_gfx_print(p_lcd, &bottom_line_centered, LCD_COLOR_WHITE, configParams.vector, font_8_point, true);
    nrf_gfx_print(p_lcd, &bottom_line, LCD_COLOR_WHITE, "OV:", font_8_point, true);

	// Start advertising and scanning
	advertising_setUser(configParams.username);
	advertising_setVector(configParams.vector);
	advertising_setDieRoll(infectionParams.diceRoll);

	advertising_init();
	advertising_start();
	scan_start();

	// Start the count down
	timerCounter = infectionParams.waitTime + MIN_WAIT_TIME;

    // Run the app forever
	// This is done in a loop for timing.  You could reconfigure it using an interrupt, but this is simple and not power hungry
	while(true){

		// Check if we should play the game
		if(timerCounter == 0){
			// Master badge function - skip playing the game if we are sending a command, so that party mode
			// can be broadcast constantly if enabled
			if(sendingCommand == false){
				playGame();
			}
		}

		// Update the LEDs based on time left in this game round
		if(timerCounter > MIN_WAIT_TIME){
			wing_leds(ON);
			helmet_leds(ON);
		}
		else if(timerCounter > (MIN_WAIT_TIME - 300)){
			helmet_leds(ON);
			wing_leds(OFF);
		}
		else{
			helmet_leds(OFF);
			wing_leds(OFF);
		}

		// Clear the seen list
		clearSeenManufacturer();

		// Clear the 801 badge list
		clearGameData();

		// Clear the command data
		clearCommandData();

		// Roll a new die
		nrf_drv_rng_rand(&rand, 1);
		infectionParams.diceRoll = rand;
		if(sendingCommand == false){
			advertising_setDieRoll(infectionParams.diceRoll);
		}

		// Draw the images
		nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, DC801UTwhite16_bmp);
		if(isButtonDown(USER_BUTTON_1)){
			masterCommandHandle();
		}
		nrf_delay_ms(1000);
		if(isButtonDown(USER_BUTTON_1)){
			masterCommandHandle();
		}
		nrf_delay_ms(1000);

		// Party mode!!!
		if(commandData.partyMode){
			nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, party_bmp);
			for(int i = 0; i < 100; i++){
				if(isButtonDown(USER_BUTTON_1)){
					masterCommandHandle();
				}
				helmet_leds(ON);
				wing_leds(ON);
				nrf_delay_ms(50);
				helmet_leds(ON);
				wing_leds(OFF);
				nrf_delay_ms(50);
				helmet_leds(OFF);
				wing_leds(ON);
				nrf_delay_ms(50);
				helmet_leds(OFF);
				wing_leds(OFF);
				nrf_delay_ms(50);
			}
		}

		if(commandData.fullReconfig){
			// Clear the configured flag
			// Next reboot, the user will have a 'fresh' badge
			storage_delete(FILE_CONFIGURATION, RECORD_CONFIGURED);
		}


		// Nearby badge shout-outs
		if(getSeenManufacturer(MANU_DCZIA_SHOTBOT)){
			nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, shotbot_bmp);
			if(isButtonDown(USER_BUTTON_1)){
				masterCommandHandle();
			}
			nrf_delay_ms(1000);
		}

		if(getSeenManufacturer(MANU_BENDER)){
			nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, bender_bmp);
			if(isButtonDown(USER_BUTTON_1)){
				masterCommandHandle();
			}
			nrf_delay_ms(1000);
		}

		if(getSeenManufacturer(MANU_CRYPTO)){
			nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, crypto_village_bmp);
			if(isButtonDown(USER_BUTTON_1)){
				masterCommandHandle();
			}
			nrf_delay_ms(1000);
		}

//		if(getSeenManufacturer(MANU_DC503)){
//			nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, dc503_bmp);
//			nrf_delay_ms(1000);
//		}

		if(getSeenManufacturer(MANU_QUEERCON)){
			nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, queercon_bmp);
			if(isButtonDown(USER_BUTTON_1)){
				masterCommandHandle();
			}
			nrf_delay_ms(1000);
		}

		if(getSeenManufacturer(MANU_DCZIA)){
			nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, dczia_bmp);
			if(isButtonDown(USER_BUTTON_1)){
				masterCommandHandle();
			}
			nrf_delay_ms(1000);
		}

	}

}

/**
 * @brief Handle commands on master badges
 * @note Define must be enabled for the functions here to work
 */
void masterCommandHandle(void){

	// If you are not a master badge, then you don't get this function.
	if(!MASTER_BADGE){
		return;
	}

	// Clear the screen and put up the header
	nrf_gfx_screen_fill(p_lcd, LCD_COLOR_BLACK);

	nrf_gfx_print(p_lcd, &top_line,
			LCD_COLOR_WHITE, "HACKS\n"
						"\n"
						"B1 Reset Nearby\n"
						"B2 Enable Party\n"
						"B3 Disable Party\n"
						"B4 Exit", font_8_point, true);


	bool done = false, change = false;
	char *string;

	// Wait until button 1 is released, which we used to enter this function
	while(isButtonDown(USER_BUTTON_1));

	while(!done){

		if(isButtonDown(USER_BUTTON_1)){
			// Kind of dangerous - resets all badges within range back to default
			string = "Sent Reset";
			advertising_setVector(COMMAND_KEYWORD);
			advertising_setFlag(COMMAND_RESET);
			change = true;
			// Only broadcast this for as long as button 1 is down to limit how many
			// badges are reset
			while(isButtonDown(USER_BUTTON_1));
			advertising_setFlag(COMMAND_NONE);
		}
		if(isButtonDown(USER_BUTTON_2)){
			// Broadcast party mode
			string = "Party Enabled";
			advertising_setVector(COMMAND_KEYWORD);
			advertising_setFlag(COMMAND_PARTY);
			change = true;
			sendingCommand = true;
			while(isButtonDown(USER_BUTTON_2));
		}
		if(isButtonDown(USER_BUTTON_3)){
			// Disable sending party mode and go back to normal
			string = "Party Disabled";
			advertising_setVector(configParams.vector);
			advertising_setFlag(COMMAND_NONE);
			change = true;
			sendingCommand = false;
			while(isButtonDown(USER_BUTTON_3));
		}
		if(isButtonDown(USER_BUTTON_4)){
			// Done, exit function
			done = true;
			while(isButtonDown(USER_BUTTON_4));
		}

		if(change){
			// Update the tag line at the bottom of the screen
			change = false;
			nrf_gfx_rect_draw(p_lcd, &bottom_line_box, 0, LCD_COLOR_BLACK, true);
			nrf_gfx_print(p_lcd, &bottom_line,
			LCD_COLOR_WHITE, string, font_8_point, true);
		}

	}

	// Clear screen and set it back up
	nrf_gfx_screen_fill(p_lcd, LCD_COLOR_BLACK);

	// Show the username on the screen
    nrf_gfx_print(p_lcd, &top_line_centered, LCD_COLOR_WHITE, configParams.username, font_8_point, true);
    nrf_gfx_print(p_lcd, &top_line, LCD_COLOR_WHITE, "  @", font_8_point, true);
    nrf_gfx_print(p_lcd, &bottom_line_centered, LCD_COLOR_WHITE, configParams.vector, font_8_point, true);
    nrf_gfx_print(p_lcd, &bottom_line, LCD_COLOR_WHITE, "OV:", font_8_point, true);

    // Once we jump back to the normal function, the center image will be displayed

}


/**
 * @brief Setup a fresh badge and save to flash
 */
void badgeSetup(void){

	bool not_configured = true;
	nrf_gfx_point_t setup_info_line = NRF_GFX_POINT(5, 20);

	char user[9];
	char vector[9];

	int configured = 0;
	int button;

	memset(user, 0, sizeof(user));
	memset(vector, 0, sizeof(vector));

	printf("New badge configure\n");

	// Clear the screen and put up the header
	nrf_gfx_screen_fill(p_lcd, LCD_COLOR_BLACK);

	nrf_gfx_print(p_lcd, &top_line,
			LCD_COLOR_WHITE, "Badge Setup", font_8_point, true);


	// Get the username
	nrf_gfx_rect_draw(p_lcd, &bottom_line_box, 0, LCD_COLOR_BLACK, true);
	nrf_gfx_print(p_lcd, &bottom_line,
			LCD_COLOR_WHITE, "+   -   Ent  Back", font_8_point, true);
	nrf_gfx_print(p_lcd, &setup_info_line,
			LCD_COLOR_WHITE, "User name?\n"
					"Hold Ent to save\n"
					"8 chars", font_8_point, true);

	getString(user, 8, &entry_line, &entry_box);
	storage_delete(FILE_CONFIGURATION, RECORD_USERNAME);
	storage_write(FILE_CONFIGURATION, RECORD_USERNAME, user, 8);


	// If we're already configured, then prevent resetting the OV
	uint32_t data;
	if(storage_read_int(FILE_CONFIGURATION, RECORD_CONFIGURED, &data) != FDS_SUCCESS){

		// Print some stuff about the game
		nrf_gfx_rect_draw(p_lcd, &middle_image_box, 0, LCD_COLOR_BLACK, true);
		nrf_gfx_rect_draw(p_lcd, &bottom_line_box, 0, LCD_COLOR_BLACK, true);
		nrf_gfx_print(p_lcd, &bottom_line,
					LCD_COLOR_WHITE, "             Next", font_8_point, true);
		nrf_gfx_print(p_lcd, &setup_info_line, LCD_COLOR_WHITE,
				"CAUTION\n"
				"possible infectious agents present\n"
				"Generating\n"
				"Offensive Vector...", font_8_point, true);

		pauseUntilPress(USER_BUTTON_4);

		nrf_gfx_rect_draw(p_lcd, &middle_image_box, 0, LCD_COLOR_BLACK, true);
		nrf_gfx_print(p_lcd, &setup_info_line, LCD_COLOR_WHITE,
				"Vector created.\n"
				"Once you name\n"
				"this vector, it\n"
				"can't be changed.", font_8_point, true);

		pauseUntilPress(USER_BUTTON_4);


		// Get the vector name
		nrf_gfx_rect_draw(p_lcd, &bottom_line_box, 0, LCD_COLOR_BLACK, true);
		nrf_gfx_print(p_lcd, &bottom_line,
				LCD_COLOR_WHITE, "+   -   Ent  Back", font_8_point, true);
		nrf_gfx_rect_draw(p_lcd, &middle_image_box, 0, LCD_COLOR_BLACK, true);
		nrf_gfx_print(p_lcd, &setup_info_line,
				LCD_COLOR_WHITE, "Vector name?\n"
						"Hold Ent to save\n"
						"8 chars", font_8_point, true);

		getString(vector, 8, &entry_line, &entry_box);
		storage_delete(FILE_CONFIGURATION, RECORD_VECTOR);
		storage_write(FILE_CONFIGURATION, RECORD_VECTOR, vector, 8);

	}


	// Closing remarks
	nrf_gfx_rect_draw(p_lcd, &middle_image_box, 0, LCD_COLOR_BLACK, true);
	nrf_gfx_rect_draw(p_lcd, &bottom_line_box, 0, LCD_COLOR_BLACK, true);
	nrf_gfx_print(p_lcd, &setup_info_line,
			LCD_COLOR_WHITE, "Setup complete.\n"
					"Hold down the\n"
					"left button at\n"
					"power up to\n"
					"change user name", font_8_point, true);
	nrf_gfx_print(p_lcd, &bottom_line,
						LCD_COLOR_WHITE, "             Done", font_8_point, true);

	pauseUntilPress(USER_BUTTON_4);

	beep(100, 500);
	nrf_delay_ms(100);
	beep(100, 500);

	printf("Done\n");

	// Flag that configuration is complete
	configured = true;
	storage_write(FILE_CONFIGURATION, RECORD_CONFIGURED, (char*)&configured, 1);

}


/**
 * @brief Clear the list of seen badge types
 */
void clearSeenManufacturer(void){
	memset(&seen_manufacturer, 0, sizeof(seen_manufacturer));
}

/**
 * @brief See if we have seen a certain badge manufacturer
 * @param id ID to check for
 * @return true if badge has been marked as seen
 */
bool getSeenManufacturer(uint16_t id){

	switch(id){
		case MANU_CRYPTO:
			if(seen_manufacturer.crypto){
				return(true);
			}
			break;
		case MANU_QUEERCON:
			if(seen_manufacturer.queercon){
				return(true);
			}
			break;
		case MANU_DC801:
			if(seen_manufacturer.dc801){
				return(true);
			}
			break;
		case MANU_DC503:
			if(seen_manufacturer.dc503){
				return(true);
			}
			break;
		case MANU_BENDER:
			if(seen_manufacturer.bender){
				return(true);
			}
			break;
		case MANU_B_D:
			if(seen_manufacturer.b_d){
				return(true);
			}
			break;
		case MANU_DCZIA:
			if(seen_manufacturer.dczia){
				return(true);
			}
			break;
		case MANU_DCZIA_SHOTBOT:
			if(seen_manufacturer.dczia_shotbot){
				return(true);
			}
			break;
		default:
			return false;
			break;
	}

	return false;
}

/**
 * @brief Saw a new badge, raise the flag
 * @param id ID of the badge type seen
 */
void addSeenManufacturer(uint16_t id){

	switch(id){
		case MANU_CRYPTO:
			seen_manufacturer.crypto = true;
			break;
		case MANU_QUEERCON:
			seen_manufacturer.queercon = true;
			break;
		case MANU_DC801:
			seen_manufacturer.dc801 = true;
			break;
		case MANU_DC503:
			seen_manufacturer.dc503 = true;
			break;
		case MANU_BENDER:
			seen_manufacturer.bender = true;
			break;
		case MANU_B_D:
			seen_manufacturer.b_d = true;
			break;
		case MANU_DCZIA:
			seen_manufacturer.dczia = true;
			break;
		case MANU_DCZIA_SHOTBOT:
			seen_manufacturer.dczia_shotbot = true;
			break;
		default:
			break;
	}
}

/**
 * @brief Clear any cached game data
 */
void clearGameData(void){
	memset(&gameData, 0, sizeof(gameData));
}

/**
 * @brief We saw a 801 badge.  Update our cache of the last seen badge.
 * @param vector The 8 char OV vector seen
 * @param diceRoll The current value being broadcast
 */
void updateGameData(char* vector, uint8_t diceRoll){

	memcpy(gameData.vector, vector, 8);
	gameData.diceRoll = diceRoll;
	gameData.nearby = true;

}

/**
 * @brief Got a command from an 801 badge
 * @param flags The command flags
 */
void updateCommandData(uint16_t flags){

//	printf("Command data is %02X\n", flags);

	if(flags == COMMAND_RESET){
		// Reset the whole badge
		commandData.fullReconfig = true;
	}
	if(flags == COMMAND_PARTY){
		// Party mode
		commandData.partyMode = true;
	}

}

/**
 * @brief Clear the command data
 */
void clearCommandData(void){
	memset(&commandData, 0, sizeof(commandData));
}

/**
 * @brief Play the game
 *
 * Game play is as follows:
 *
 * 1) Roll for a random 8 bit number
 * 2) Find the last seen random number and compare
 * 3) If ours is less, roll for another random number
 * 4) If that roll is less than 200, then we lose and swap teams
 * 		- this is to try to keep someone from taking over if they figure out the protocol
 * 5) If we win either roll, keep our team and return
 *
 */
void playGame(void){

	uint8_t rand = 0;

	// Time's up!  Let's see what happens
	printf("Playing game\n");

	// Draw the game image
	nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, DC801UTwhite16_bmp);

	// Flash the eyes
	for(int i = 0; i < 8; i++){
		set_eye_color(eye_side_both, eye_color_none);
		beep(100, 800);
		set_eye_color(eye_side_both, eye_color_green);
		beep(100, 700);
	}

	nrf_delay_ms(1000);

	if(!gameData.nearby){
		// No one nearby, win by default
		printf("No one nearby\n");
		nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, game_win_bmp);
		set_eye_color(eye_side_both, eye_color_all);
	}
	else if(infectionParams.diceRoll < gameData.diceRoll){
		// I lost this one.  Roll to see if I change...
		nrf_drv_rng_rand(&rand, 1);

// Master badges can't get updated, in an attempt to balance the game against the default value
#if !MASTER_BADGE
		if(rand < 200){
			// Yup, I lost, change it
			printf("Changing vector\n");

			// Is it the same as what I have now?
			if((strncmp(gameData.vector, configParams.vector, 8) == 0)){
				// It's the same, show the won image after all
				nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, game_win_bmp);
				set_eye_color(eye_side_both, eye_color_all);
			}
			else{
				// Draw the lost image
				nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, game_lost_bmp);
				set_eye_color(eye_side_both, eye_color_red);

				// Update
				memcpy(configParams.vector, gameData.vector, 8);
				storage_delete(FILE_CONFIGURATION, RECORD_VECTOR);
				storage_write(FILE_CONFIGURATION, RECORD_VECTOR, configParams.vector, 8);
				advertising_setVector(configParams.vector);
				nrf_gfx_rect_draw(p_lcd, &bottom_line_box, 0, LCD_COLOR_BLACK, true);
				nrf_gfx_print(p_lcd, &bottom_line_centered, LCD_COLOR_WHITE, configParams.vector, font_8_point, true);
				nrf_gfx_print(p_lcd, &bottom_line, LCD_COLOR_WHITE, "OV:", font_8_point, true);
			}
		}
		else{
#endif
			// In the clear!
			// Draw the won image
			printf("I won!\n");
			nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, game_win_bmp);
			set_eye_color(eye_side_both, eye_color_all);
#if !MASTER_BADGE
		}
#endif
	}
	else{
		nrf_gfx_bmp565_draw(p_lcd, &middle_image_box, game_win_bmp);
		set_eye_color(eye_side_both, eye_color_all);
	}

	nrf_delay_ms(1500);
	set_eye_color(eye_side_both, eye_color_none);

	// Roll a new timer dice
	nrf_drv_rng_rand(&rand, 1);
	infectionParams.waitTime = rand;
	timerCounter = infectionParams.waitTime + MIN_WAIT_TIME;

}

