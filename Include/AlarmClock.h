// ******** AlarmClock.h **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: Header file for AlarmClock.c
// Lab Number: Lab03
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#ifndef ALARM_CLOCK_H_
#define ALARM_CLOCK_H_

#include <stdint.h>
#include <stdbool.h>

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void); // Enable interrupts

typedef enum {
	ButtonZeroPressed,
	ButtonOnePressed,
	ButtonTwoPressed,
	ButtonThreePressed,
	TimeChanged
} StateChange;

typedef enum {
    Analog,
    Digital
} DisplayMode;

/**
 * Handler for when external state such as button push or time change occurs.
 * @param: reason: Reason for why the state changed.
 */
void AlarmClock_DisplayShouldUpdate(StateChange change);

/**
 * Force display to redraw.
 */
void AlarmClock_RedrawDisplay(void);

/**
 * Set display mode and navigate to that display.
 * @param: mode: New display mode.
 */
void AlarmClock_SetDisplayMode(DisplayMode);

extern bool AlarmEnabled;
extern uint8_t AlarmTimeHours;
extern uint8_t AlarmTimeMinutes;

#endif // ALARM_CLOCK_H_
