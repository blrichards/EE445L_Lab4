// ******** VirtualPins.h **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: Defines virtual pin numbers and their uses.
// Lab Number: Lab04
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#include <stdint.h>

#ifndef VIRTUAL_PINS_H_
#define VIRTUAL_PINS_H_

// From Blynk
#define NUM_VIRTUAL_PINS_FROM_BLYNK 5
#define VP_DISPLAY_MODE_SELECTOR 0
#define VP_FAR_LEFT_SWITCH 1
#define VP_SECOND_TO_LEFT_SWITCH 2
#define VP_SECOND_TO_RIGHT_SWITCH 3
#define VP_FAR_RIGHT_SWITCH 4

// To Blynk
#define NUM_VIRTUAL_PINS_TO_BLYNK 3
#define TO_BLYNK_VIRTUAL_PIN_OFFSET 70
#define VP_CURRENT_HOURS 70
#define VP_CURRENT_MINUTES 71
#define VP_CURRENT_SECONDS 72

typedef void(*FromBlynkHandler)(int32_t);
typedef void(*ToBlynkHandler)(void);

void VirtualPins_Init(void);

extern FromBlynkHandler FromBlynkHandlers[NUM_VIRTUAL_PINS_FROM_BLYNK];
extern ToBlynkHandler ToBlynkHandlers[NUM_VIRTUAL_PINS_TO_BLYNK];

#endif // VIRTUAL_PINS_H_
