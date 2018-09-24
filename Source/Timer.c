// ******** Timer.c **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: File for timer functionality
// Lab Number: Lab03
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#include "Timer.h"
#include "AlarmClock.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include <stdint.h>

#define PB7 (*((volatile uint32_t*)0x40005200))
#define PF2 (*((volatile uint32_t*)0x40025010))
	
uint8_t CurrentSeconds = 0;
uint8_t CurrentMinutes = 0;
uint8_t CurrentHours = 0;

void SysTick_Init(void)
{
	NVIC_ST_CTRL_R = 0;           		// disable SysTick during setup
	NVIC_ST_RELOAD_R = 79999;       	// reload value for 1kHz
	NVIC_ST_CURRENT_R = 0;        		// any write to current clears it
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x00000000; // priority 1
	NVIC_ST_CTRL_R = 0x00000006;  		// enable interrupts, not clock
}

void SysTick_Handler(void){
    NVIC_ST_RELOAD_R = 79999;     // reload value for high phase
	PB7 ^= 0x80;
	PF2 ^= 0x04;
}

void Timer0A_Init(uint32_t reloadValue)
{
    volatile uint32_t delay;
    SYSCTL_RCGCTIMER_R |= 0x01;			 	// activate timer0
    delay = SYSCTL_RCGCTIMER_R; 			// allow time to finish activating
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;		// disable timer0A during setup
    TIMER0_CFG_R = 0; 						// configure for 32-bit timer mode
	
    // configure for periodic mode
    TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
    TIMER0_TAILR_R = reloadValue; 			// start value for 1 Hz interrupts
    TIMER0_IMR_R |= TIMER_IMR_TATOIM; 		// enable timeout (rollover) interrupt
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; 		// clear timer0A timeout flag
    TIMER0_CTL_R |= TIMER_CTL_TAEN; 		// enable timer0A 32-b, periodic, interrupts
    
    // Timer0A=priority 2
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x40000000; // top 3 bits
    NVIC_EN0_R = 1 << 19; // enable interrupt 19 in NVIC
}

void Timer0A_Handler(void)
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT; // acknowledge timer0A timeout
	CurrentSeconds++;
	
	if((CurrentSeconds % 60) == 0){
		CurrentSeconds = 0;
		CurrentMinutes++;
		if((CurrentMinutes % 60) == 0){
			CurrentMinutes = 0;
			CurrentHours++;
			if((CurrentHours % 24) == 0)
				CurrentHours = 0;
		}
	}
	
	AlarmClock_DisplayShouldUpdate(TimeChanged);
}

void Timer1_Init(uint32_t reloadValue)
{
    volatile uint32_t delay;
    SYSCTL_RCGCTIMER_R |= 0x02; // 0) activate TIMER1
    delay = SYSCTL_RCGCTIMER_R; // allow time to finish activating
    TIMER1_CTL_R = 0x00000000; // 1) disable TIMER1A during setup
    TIMER1_CFG_R = 0x00000000; // 2) configure for 32-bit mode
    TIMER1_TAMR_R = 0x00000002; // 3) configure for periodic mode, down-count
    TIMER1_TAILR_R = reloadValue; // 4) reload value
    TIMER1_TAPR_R = 0; // 5) bus clock resolution
    TIMER1_CTL_R = 0x00000001; // 10) enable TIMER1A
}
