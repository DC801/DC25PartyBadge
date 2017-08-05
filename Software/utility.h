/*
 * @file utility.h
 *
 * @date Jul 24, 2017
 * @author hamster
 *
 * Utility functions
 *
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#define BUTTON_PRESSED 	0
#define BUTTON_RELEASED 1
#define BUTTON_DEBOUNCE_MS		15
#define BUTTON_LONG_PRESS_MS	200

// For logging to the real time terminal via JLink SWD RTT
#define RTT_PRINTF(...) \
do { \
     char str[64];\
     sprintf(str, __VA_ARGS__);\
     SEGGER_RTT_WriteString(0, str);\
 } while(0)

#define printf RTT_PRINTF

#define FIRST_ALLOWED_CHAR	0x20
#define LAST_ALLOWED_CHAR	0x7E
#define FIRST_LETTER		'A'
#define LAST_LETTER 		'z'

int getButton(void);
void getString(char* string, int len, nrf_gfx_point_t *point, nrf_gfx_rect_t *box);
bool isButtonDown(int button);
void pauseUntilPress(int button);
void beep(int duration, int frequency);
bool stringIsBlank(char *string, int len);


#endif /* UTILITY_H_ */
