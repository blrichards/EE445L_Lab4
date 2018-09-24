// ******** Speaker.c **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: Main file for speaker functionality
// Lab Number: Lab03
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#include "Speaker.h"
#include "Timer.h"
#include "tm4c123gh6pm.h"

#define PB7 (*((volatile uint32_t*)0x40005200))

void Speaker_Init(void)
{
	GPIO_PORTB_DIR_R |= 0x80;    		// Set dir to output 7
	GPIO_PORTB_AFSEL_R &= ~0x80;  		// disable alt funct
	GPIO_PORTB_DEN_R |= 0x80;     		// enable digital I/O
	GPIO_PORTB_PCTL_R &= ~0x000F0000;   // configure PF4 as GPIO
	GPIO_PORTB_AMSEL_R = 0;       		// disable analog functionality on PF
	SysTick_Init();
}

void Speaker_Enable(void)
{
	NVIC_ST_CTRL_R |= 0x01;		//Enables SysTick
}

void Speaker_Disable(void)
{
	NVIC_ST_CTRL_R &= ~0x01;	//Disables SysTick
	PB7 &= ~0x80;
}
