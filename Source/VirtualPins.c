#include "Timer.h"

#include "AlarmClock.h"
#include "Blynk.h"
#include "VirtualPins.h"

#define ANALOG_CLOCK_MODE 0
#define DIGITAL_CLOCK_MODE 1

FromBlynkHandler FromBlynkHandlers[NUM_VIRTUAL_PINS_FROM_BLYNK];
ToBlynkHandler ToBlynkHandlers[NUM_VIRTUAL_PINS_TO_BLYNK];

////////////////////////////
//  From Blynk Handlers   //
////////////////////////////
static void setDisplayMode(int32_t mode) 
{
	if (mode == ANALOG_CLOCK_MODE)
		AlarmClock_SetDisplayMode(Analog);
	else if (mode == DIGITAL_CLOCK_MODE)
		AlarmClock_SetDisplayMode(Digital);
}

static void buttonZeroPressed(int32_t unused)
{
	AlarmClock_DisplayShouldUpdate(ButtonZeroPressed);
}

static void buttonOnePressed(int32_t unused)
{
	AlarmClock_DisplayShouldUpdate(ButtonOnePressed);
}

static void buttonTwoPressed(int32_t unused)
{
	AlarmClock_DisplayShouldUpdate(ButtonTwoPressed);
}

static void buttonThreePressed(int32_t unused)
{
	AlarmClock_DisplayShouldUpdate(ButtonThreePressed);
}

////////////////////////////
//   To Blynk Handlers    //
////////////////////////////
static void sendCurrentHours(void) 
{
	TM4C_to_Blynk(VP_CURRENT_HOURS, CurrentHours);
}

static void sendCurrentMinutes(void) 
{
	TM4C_to_Blynk(VP_CURRENT_MINUTES, CurrentMinutes);
}

static void sendCurrentSeconds(void) 
{
	TM4C_to_Blynk(VP_CURRENT_SECONDS, CurrentSeconds);
}

static void fromBlynkInit(void)
{
	FromBlynkHandlers[VP_DISPLAY_MODE_SELECTOR] = setDisplayMode;
	FromBlynkHandlers[VP_FAR_LEFT_SWITCH] = buttonZeroPressed;
	FromBlynkHandlers[VP_SECOND_TO_LEFT_SWITCH] = buttonOnePressed;
	FromBlynkHandlers[VP_SECOND_TO_RIGHT_SWITCH] = buttonTwoPressed;
	FromBlynkHandlers[VP_FAR_RIGHT_SWITCH] = buttonThreePressed;
}

static void toBlynkInit(void)
{
	ToBlynkHandlers[VP_CURRENT_HOURS - TO_BLYNK_VIRTUAL_PIN_OFFSET] = sendCurrentHours;
	ToBlynkHandlers[VP_CURRENT_MINUTES - TO_BLYNK_VIRTUAL_PIN_OFFSET] = sendCurrentMinutes;
	ToBlynkHandlers[VP_CURRENT_SECONDS - TO_BLYNK_VIRTUAL_PIN_OFFSET] = sendCurrentSeconds;
}

void VirtualPins_Init(void)
{
	fromBlynkInit();
	toBlynkInit();
}
