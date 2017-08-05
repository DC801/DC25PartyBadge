/*
 * @file test.h
 *
 * @file May 24, 2017
 * @author hamster
 *
 * @brief Test functions header file
 *
 */

#ifndef TEST_H_
#define TEST_H_

typedef enum {
	test_mode_none,
	test_mode_eyes,
	test_mode_helmet,
	test_mode_wingss,
	test_mode_speaker,
	test_mode_lcd,
	test_mode_button
} TEST_MODES;

typedef enum {
	run_mode_none,
	run_mode_single,
	run_mode_all
} RUN_MODE;


void test_wing_leds(void);
void test_helmet_leds(void);
void test_eye_leds(void);
void test_lcd(void);
void test_speaker(void);
void run_test_mode(void);
void draw_menu(void);

#endif /* TEST_H_ */
