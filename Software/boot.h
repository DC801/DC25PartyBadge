/**
 *
 * @file boot.c
 *
 * @date May 24, 2017
 * @author hamster
 *
 */

#ifndef BOOT_H_
#define BOOT_H_

APP_TIMER_DEF(gameTimerID);
void timeout_handler(void * p_context);

extern volatile uint16_t timerCounter;

#endif /* BOOT_H_ */
