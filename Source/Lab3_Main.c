// ******** Lab3_Main.c **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: Main file for Lab 3
// Lab Number: Lab03
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#include "AlarmClock.h"
#include "Buttons.h"
#include "Debug.h"
#include "PLL.h"
#include "Speaker.h"
#include "ST7735.h"
#include "Timer.h"
#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define PF2 (*((volatile uint32_t*)0x40025010))
#define PF1 (*((volatile uint32_t*)0x40025008))

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void); // Enable interrupts
long StartCritical(void); // previous I bit, disable interrupts
void EndCritical(long sr); // restore I bit to previous value
void WaitForInterrupt(void); // low power mode

/*
int main(void)
{
	PLL_Init(Bus80MHz); // 80 MHz
    SYSCTL_RCGCGPIO_R |= 0x32; // activate port F, E, B

    // configure PF2 as GPIO
    Debug_Init();
    Output_Init();
	Buttons_Init();
	Timer0A_Init(79999999);
	Speaker_Init();
    EnableInterrupts();
	
	AlarmClock_RedrawDisplay();
	
    while (true) {
		PF1 ^= 0x02; // toggles when running in main
    }
}
*/
