// ******** AlarmClock.c **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: File for the overarching
// alarm clock functionality
// Lab Number: Lab03
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#include "AlarmClock.h"
#include "Graphics.h"
#include "ST7735.h"
#include "Speaker.h"
#include "Timer.h"
#include "Graphics.h"
#include "ST7735.h"
#include "Timer.h"
#include "tm4c123gh6pm.h"
#include <math.h>
#include <stdio.h>

#define NUM_BUTTONS 4
#define NUM_HOURS 24
#define NUM_MINUTES 60
#define NUM_SECONDS 60
#define PI 3.14159265
#define NUM_CLOCK_HOURS 12

#define CONVERT_X_FOR_DRAW_STRING(x) ((uint16_t)(20.0 * ((x) / (float)ST7735_TFTWIDTH)))
#define CONVERT_Y_FOR_DRAW_STRING(y) ((uint16_t)(15.0 * ((y) / (float)ST7735_TFTHEIGHT)))

#define PF2 (*((volatile uint32_t*)0x40025010))

typedef void (*StateAction)(void);
typedef void (*StateChangedHandler)(void);

typedef enum {
    MainMenu,
    AnalogDisplay,
    DigitalDisplay,
    SetDisplayMode,
    SetTime,
    AlarmSettings,
    AlarmOn
} NavigationState;

typedef enum {
    Clock,
    Alarm
} TimeToSet;

typedef struct {
    uint16_t x;
    uint16_t y;
} Point;

////////////////////////////
//    UI Declarations     //
////////////////////////////
static void drawMainMenu(void);
static void drawAnalogDisplay(void);
static void drawDigitalDisplay(void);
static void drawSetDisplayMode(void);
static void drawSetTime(void);
static void drawAlarmSettings(void);
static void drawAlarmOn(void);

////////////////////////////
//       UI Mappings      //
////////////////////////////
static const StateChangedHandler NavigationStateChangedHandler[] = {
    drawMainMenu,
    drawAnalogDisplay,
    drawDigitalDisplay,
    drawSetDisplayMode,
    drawSetTime,
    drawAlarmSettings,
    drawAlarmOn
};

////////////////////////////
//  Action Declarations   //
////////////////////////////
static void nullAction(void);
static void goToDisplayModeSettingsAction(void);
static void goToAlarmSettingsAction(void);
static void goToDisplayAction(void);
static void goToSetClockTimeAction(void);
static void goToMainMenuAction(void);
static void setDisplayModeToAnalogAction(void);
static void setDisplayModeToDigitalAction(void);
static void incrementSetTimeHoursAction(void);
static void incrementSetTimeMinutesAction(void);
static void goToSetAlarmTimeAction(void);
static void toggleAlarmEnabledAction(void);
static void confirmSetTimeAction(void);
static void alarmOffAction(void);

////////////////////////////
//    Action Mappings     //
////////////////////////////
static const StateAction MainMenuActions[NUM_BUTTONS] = {
    goToDisplayAction,
    goToDisplayModeSettingsAction,
    goToSetClockTimeAction,
    goToAlarmSettingsAction
};
static const StateAction AnalogDisplayActions[NUM_BUTTONS] = {
    goToMainMenuAction,
    goToMainMenuAction,
    goToMainMenuAction,
    goToMainMenuAction
};
static const StateAction DigitalDisplayActions[NUM_BUTTONS] = {
    goToMainMenuAction,
    goToMainMenuAction,
    goToMainMenuAction,
    goToMainMenuAction
};
static const StateAction SetDisplayModeActions[NUM_BUTTONS] = {
    setDisplayModeToAnalogAction,
    setDisplayModeToDigitalAction,
    goToMainMenuAction,
    nullAction
};
static const StateAction SetTimeActions[NUM_BUTTONS] = {
    incrementSetTimeHoursAction,
    incrementSetTimeMinutesAction,
    confirmSetTimeAction,
    goToMainMenuAction
};
static const StateAction AlarmSettingsActions[NUM_BUTTONS] = {
    goToSetAlarmTimeAction,
    toggleAlarmEnabledAction,
    goToMainMenuAction,
    nullAction
};
static const StateAction AlarmOnActions[NUM_BUTTONS] = {
    alarmOffAction,
    alarmOffAction,
    alarmOffAction,
    alarmOffAction
};
static const StateAction* NavigationStateActions[] = {
    MainMenuActions,
    AnalogDisplayActions,
    DigitalDisplayActions,
    SetDisplayModeActions,
    SetTimeActions,
    AlarmSettingsActions,
    AlarmOnActions
};

////////////////////////////
//    State Variables     //
////////////////////////////
static NavigationState currentState = AnalogDisplay;
static DisplayMode currentDisplayMode = Analog;
static uint8_t setTimeHours = 0;
static uint8_t setTimeMinutes = 0;
static NavigationState prevState = MainMenu;
static TimeToSet currentTimeToSet = Clock;
static bool shouldClearScreen = true;
static const uint16_t clockFaceCenterX = (ST7735_TFTWIDTH / 2);
static const uint16_t clockFaceCenterY = (ST7735_TFTHEIGHT / 2);
static const uint16_t clockFaceRadius = ST7735_TFTWIDTH / 2.1;
static const Point clockFaceNumbersPoints[4] = {
    { CONVERT_X_FOR_DRAW_STRING(clockFaceCenterX + clockFaceRadius), CONVERT_Y_FOR_DRAW_STRING(clockFaceCenterY) + 1 },
    { CONVERT_X_FOR_DRAW_STRING(clockFaceCenterX), CONVERT_Y_FOR_DRAW_STRING(clockFaceCenterY + clockFaceRadius) },
    { CONVERT_X_FOR_DRAW_STRING(clockFaceCenterX - clockFaceRadius) + 2, CONVERT_Y_FOR_DRAW_STRING(clockFaceCenterY) + 1 },
    { CONVERT_X_FOR_DRAW_STRING(clockFaceCenterX), CONVERT_Y_FOR_DRAW_STRING(clockFaceCenterY - clockFaceRadius) + 2 }
};
static const char* clockFaceNumbers[4] = { "3", "6", "9", "12" };
static size_t numClockFaceNumbers = sizeof(clockFaceNumbers) / sizeof(const char*);
static const uint16_t hoursHandLength = clockFaceRadius / 3.0;
static const uint16_t minutesHandLength = hoursHandLength * 1.5;
static const uint16_t secondsHandLength = hoursHandLength * 2;
static Point prevHoursPoint = { 0 };
static Point prevMinutesPoint = { 0 };
static Point prevSecondsPoint = { 0 };
static const uint16_t clockFaceBackground = ST7735_BLACK;
static const uint16_t hoursHandColor = ST7735_BURNT_ORANGE;
static const uint16_t minutesHandColor = ST7735_WHITE;
static const uint16_t secondsHandColor = ST7735_GRAY;

////////////////////////////
//    Global Variables    //
////////////////////////////
bool AlarmEnabled = false;
uint8_t AlarmTimeHours = 0;
uint8_t AlarmTimeMinutes = 0;

////////////////////////////
//   Alarm Clock Logic    //
////////////////////////////
static void updateDisplay(void)
{
    NavigationStateChangedHandler[currentState]();
}

void AlarmClock_RedrawDisplay(void)
{
    shouldClearScreen = true;
    updateDisplay();
}

void AlarmClock_DisplayShouldUpdate(StateChange change)
{
    switch (change) {
    case TimeChanged:
        if (currentState != AnalogDisplay && currentState != DigitalDisplay)
            return;
        if (AlarmEnabled && CurrentHours == AlarmTimeHours && CurrentMinutes == AlarmTimeMinutes && CurrentSeconds == 0) {
            currentState = AlarmOn;
            Speaker_Enable();
        }
        updateDisplay();
        break;
    default:
        NavigationStateActions[currentState][change]();
        break;
    }
}

void AlarmClock_SetDisplayMode(DisplayMode mode)
{
	DisableInterrupts();
	currentDisplayMode = mode;
	if (currentState == AnalogDisplay || currentState == DigitalDisplay) {
		currentState = mode == Analog ? AnalogDisplay : DigitalDisplay;
		AlarmClock_RedrawDisplay();
	}
	EnableInterrupts();
}

////////////////////////////
//   UI Implementations   //
////////////////////////////
void drawMainMenu(void)
{
    Graphics_ClearDisplay();
    ST7735_OutString("1. Display Clock\n");
    ST7735_OutString("2. Set Display Mode\n");
    ST7735_OutString("3. Set Time\n");
    ST7735_OutString("4. Set Alarm\n");
}

static void drawAnalogClockFace(void)
{
    ST7735_Circle(clockFaceCenterX, clockFaceCenterY, clockFaceRadius, ST7735_BURNT_ORANGE);
    for (size_t i = 0; i < numClockFaceNumbers; ++i) {
        Point p = clockFaceNumbersPoints[i];
        ST7735_DrawString(p.x, p.y, clockFaceNumbers[i], ST7735_BURNT_ORANGE);
    }
}

static void drawAnalogClockHand(uint8_t current, uint8_t total, uint16_t length, uint16_t color, Point* prevPoint)
{
    double angle = (current / (double)total) * 2 * PI;

    uint16_t x = clockFaceCenterX + (length * sin(angle));
    uint16_t y = clockFaceCenterY - (length * cos(angle));
    if (prevPoint)
        ST7735_Line(clockFaceCenterX, clockFaceCenterY, prevPoint->x, prevPoint->y, clockFaceBackground);
    ST7735_Line(clockFaceCenterX, clockFaceCenterY, x, y, color);
    prevPoint->x = x;
    prevPoint->y = y;
}

void drawAnalogDisplay(void)
{
    if (shouldClearScreen) {
        Graphics_ClearDisplay();
        shouldClearScreen = false;
    }

    drawAnalogClockFace();
    drawAnalogClockHand(CurrentSeconds, NUM_SECONDS, secondsHandLength, secondsHandColor, &prevSecondsPoint);
    drawAnalogClockHand(CurrentMinutes, NUM_MINUTES, minutesHandLength, minutesHandColor, &prevMinutesPoint);
	drawAnalogClockHand(CurrentHours, NUM_CLOCK_HOURS, hoursHandLength, hoursHandColor, &prevHoursPoint);
	
	(CurrentHours < 12) ? ST7735_DrawString(19,15,"AM",ST7735_BURNT_ORANGE) : ST7735_DrawString(19,15,"PM",ST7735_BURNT_ORANGE);
}

void drawDigitalDisplay(void)
{
	if (shouldClearScreen) {
		Graphics_ClearDisplay();
		ST7735_OutString("Current Time:\n");
		shouldClearScreen = false;
	} else ST7735_FillRect(0, 8, 40, 8, ST7735_BLACK);
	
	ST7735_SetCursor(0, 1);
	if(CurrentHours >= 13){
		if(CurrentHours <= 21) ST7735_OutString("0");
		ST7735_OutUDec(CurrentHours - 12);
	} else{
		if(CurrentHours <= 9 && CurrentHours != 0){
			ST7735_OutString("0");
			ST7735_OutUDec(CurrentHours);
		}
		else if (CurrentHours == 0) ST7735_OutString("12");
		else ST7735_OutUDec(CurrentHours);
	}
	
	ST7735_OutString(":");
	
	if(CurrentMinutes <= 9) 
		ST7735_OutString("0");
	ST7735_OutUDec(CurrentMinutes);
	ST7735_OutString(":");
	if(CurrentSeconds <= 9) 
		ST7735_OutString("0");
	ST7735_OutUDec(CurrentSeconds);
	
	if (CurrentHours < 12)
		ST7735_OutString(" AM");
	else
		ST7735_OutString(" PM");
}

void drawSetDisplayMode(void)
{
    if (shouldClearScreen) {
		Graphics_ClearDisplay();
		shouldClearScreen = false;
	}
	ST7735_OutString("1. Set Analog\n");
	ST7735_OutString("2. Set Digital\n");
	ST7735_OutString("3. Goto Main Menu\n");
}

void drawSetTime(void)
{
    if (shouldClearScreen) {
		Graphics_ClearDisplay();
		ST7735_SetCursor(0, 4);
		ST7735_OutString("3) Confirm\n");
		ST7735_OutString("4) Cancel\n");
		shouldClearScreen = false;
	} else ST7735_FillRect(0, 0, 90, 3, ST7735_BLACK);
	
	ST7735_SetCursor(0, 0);
	char buf[256];
	sprintf(buf, "1) Add Hour\n    current: %d\n", setTimeHours);
	ST7735_OutString(buf);
	sprintf(buf, "2) Add Minute\n    current: %d", setTimeMinutes);
	ST7735_OutString(buf);
}

void drawAlarmSettings(void)
{
    if (shouldClearScreen) {
		Graphics_ClearDisplay();
		shouldClearScreen = false;
	} else ST7735_FillRect(0, 0, 60, 80, ST7735_BLACK);
	
	ST7735_SetCursor(0, 0);
	ST7735_OutString("1) Set Alarm\n");
	char buf[256];
	sprintf(buf, "2) %s Alarm\n", AlarmEnabled ? "Turn Off" : "Turn On");
	ST7735_OutString(buf);
	ST7735_OutString("3) Return\n");
}

void drawAlarmOn(void)
{
    Graphics_ClearDisplay();
    ST7735_OutString("Alarm is on!!!!\n");
}

////////////////////////////
// Action Implementations //
////////////////////////////
void nullAction(void)
{
    // This action is a placeholder for buttons that don't do
    // anything on a given screen.
}

void goToDisplayModeSettingsAction(void)
{
    currentState = SetDisplayMode;
    AlarmClock_RedrawDisplay();
}

void goToAlarmSettingsAction(void)
{
    currentState = AlarmSettings;
    AlarmClock_RedrawDisplay();
}

void goToDisplayAction(void)
{
    if (currentDisplayMode == Analog)
        currentState = AnalogDisplay;
    else
        currentState = DigitalDisplay;
    AlarmClock_RedrawDisplay();
}

void goToSetClockTimeAction(void)
{
    currentTimeToSet = Clock;
    DisableInterrupts();
    setTimeHours = CurrentHours;
    setTimeMinutes = CurrentMinutes;
    currentState = SetTime;
    AlarmClock_RedrawDisplay();
    EnableInterrupts();
}

void goToMainMenuAction(void)
{
    currentState = MainMenu;
    AlarmClock_RedrawDisplay();
}

void setDisplayModeToAnalogAction(void)
{
    currentDisplayMode = Analog;
}

void setDisplayModeToDigitalAction(void)
{
    currentDisplayMode = Digital;
}

void incrementSetTimeHoursAction(void)
{
    setTimeHours = (setTimeHours + 1) % NUM_HOURS;
    updateDisplay();
}

void incrementSetTimeMinutesAction(void)
{
    setTimeMinutes = (setTimeMinutes + 1) % NUM_MINUTES;
    updateDisplay();
}

void confirmSetTimeAction(void)
{
    DisableInterrupts();
    if (currentTimeToSet == Clock) {
        CurrentHours = setTimeHours;
        CurrentMinutes = setTimeMinutes;
        CurrentSeconds = 0;
        currentState = MainMenu;
    } else if (currentTimeToSet == Alarm) {
        AlarmTimeHours = setTimeHours;
        AlarmTimeMinutes = setTimeMinutes;
        currentState = AlarmSettings;
    }
    updateDisplay();
    EnableInterrupts();
}

void goToSetAlarmTimeAction(void)
{
    currentTimeToSet = Alarm;
    setTimeHours = AlarmTimeHours;
    setTimeMinutes = AlarmTimeMinutes;
    currentState = SetTime;
    AlarmClock_RedrawDisplay();
}

void toggleAlarmEnabledAction(void)
{
    AlarmEnabled = !AlarmEnabled;
    updateDisplay();
}

void alarmOffAction(void)
{
    Speaker_Disable();
    currentState = prevState;
    AlarmClock_RedrawDisplay();
}
