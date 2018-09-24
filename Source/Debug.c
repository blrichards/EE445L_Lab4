// ******** Debug.c **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: File for adding on board heartbeats
// Lab Number: Lab03
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#include "Debug.h"
#include "tm4c123gh6pm.h"

void Debug_Init(void) {
	GPIO_PORTF_DIR_R |= 0x06; // make PF2, PF1 out (built-in LED)
    GPIO_PORTF_AFSEL_R &= ~0x06; // disable alt funct on PF2, PF1
    GPIO_PORTF_DEN_R |= 0x06; // enable digital I/O on PF2, PF1
    GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R & 0xFFFFF00F) + 0x00000000;
    GPIO_PORTF_AMSEL_R = 0; // disable analog functionality on PF
}
