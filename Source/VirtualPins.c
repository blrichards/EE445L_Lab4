#include "Timer.h"

#include "AlarmClock.h"
#include "Blynk.h"
#include "VirtualPins.h"

#define ANALOG_CLOCK_MODE 0
#define DIGITAL_CLOCK_MODE 1

FromBlynkHandler FromBlynkHandlers[NUM_VIRTUAL_PINS_FROM_BLYNK];
ToBlynkHandler ToBlynkHandlers[NUM_VIRTUAL_PINS_TO_BLYNK];

static void setDisplayMode(int32_t mode) {
	if (mode == ANALOG_CLOCK_MODE)
		AlarmClock_SetDisplayMode(Analog);
	else if (mode == DIGITAL_CLOCK_MODE)
		AlarmClock_SetDisplayMode(Digital);
}

static void sendCurrentHours(void) {
	TM4C_to_Blynk(VP_CURRENT_HOURS, CurrentHours);
}

static void sendCurrentMinutes(void) {
	TM4C_to_Blynk(VP_CURRENT_MINUTES, CurrentMinutes);
}

static void sendCurrentSeconds(void) {
	TM4C_to_Blynk(VP_CURRENT_SECONDS, CurrentSeconds);
}

static void fromBlynkInit(void)
{
	FromBlynkHandlers[VP_FAR_LEFT_SWITCH] = AlarmClock_DisplayShouldUpdate;	//TODO - not sure if this is right
}

static void toBlynkInit(void)
{
	ToBlynkHandlers[VP_CURRENT_HOURS - TO_BLYNK_VIRTUAL_PIN_OFFSET] = sendCurrentHours;
}

void VirtualPins_Init(void)
{
	fromBlynkInit();
	toBlynkInit();
}
