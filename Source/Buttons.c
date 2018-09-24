// ******** Buttons.c **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: File for button functionality 
// Lab Number: Lab03
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#include "AlarmClock.h"
#include "Buttons.h"
#include "Speaker.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

#define FAR_LEFT_SWITCH 		0x8
#define SECOND_TO_LEFT_SWITCH 	0x4
#define SECOND_TO_RIGHT_SWITCH 	0x2
#define FAR_RIGHT_SWITCH 		0x1
#define PF2 (*((volatile uint32_t*)0x40025010))

void Buttons_Init(void)
{                      
  GPIO_PORTE_DIR_R &= ~0x0F;    // Set dir to input 0-3
  GPIO_PORTE_AFSEL_R &= ~0x0F;  // disable alt funct
  GPIO_PORTE_DEN_R |= 0x0F;     //     enable digital I/O
  GPIO_PORTE_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTE_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTE_IS_R &= ~0x0F;     // is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x0F;    // PF4 is not both edges
  GPIO_PORTE_IEV_R |= ~0x0F;    // PF4 rising edge event
  GPIO_PORTE_ICR_R = 0x0F;      // clear flag
  GPIO_PORTE_IM_R |= 0x0F;      // arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF00)|0x000000E0; // (g) priority 5
  NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 30 in NVIC
}

void GPIOPortE_Handler(void){
	uint8_t flags = (GPIO_PORTE_MIS_R & 0x0F);
	GPIO_PORTE_ICR_R = 0x0F;      // acknowledge flag

	switch(flags){
		case FAR_LEFT_SWITCH:
			AlarmClock_DisplayShouldUpdate(ButtonZeroPressed);
			break;
		case SECOND_TO_LEFT_SWITCH:
			AlarmClock_DisplayShouldUpdate(ButtonOnePressed);
			break;
		case SECOND_TO_RIGHT_SWITCH:
			AlarmClock_DisplayShouldUpdate(ButtonTwoPressed);
			break;
		case FAR_RIGHT_SWITCH:	
			AlarmClock_DisplayShouldUpdate(ButtonThreePressed);
			break;
		default:	//multiple buttons pressed, dont do anything
			break;
	}
		
	PF2 ^= 0x04;
	//TODO: to process data input to the clock
}
