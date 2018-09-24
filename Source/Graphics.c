// ******** Graphics.c **************
// Authors: Ryan Mitchell and Ben Richards
// Initial Creation Date: 9/18/18
// Description: Main file for graphic functionality
// Lab Number: Lab03
// TA: Zee Lv
// Date of last revision: 9/19/2018
// Hardware Configuration: N/A

#include "Graphics.h"
#include "ST7735.h"

void Graphics_ClearDisplay(void)
{
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_SetCursor(0, 0);
}
