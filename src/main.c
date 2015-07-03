/*
 * ---------- FIRE ALARM SYSTEM ----------
 * main.c
 *
 */


#include "commons.h"

#include "BSP/uart.h"

#include "LCD/lcd.h"




int main(int argc, char* argv[])
{
	HAL_Init(); /* Must be called before anything, related to HAL */

	#ifdef DEBUG
	InitializeUsart2();
	#endif

	Console("SystemCoreClock:", SystemCoreClock);

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2); /* 2 pre 2 sub */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0); /* SysTick highest priority */

	SysTick_Config(SystemCoreClock / SYSTICK_FREQ); /* Configure SysTick as 1kHz */
	Console("SysTick Frequency(Hz):", SYSTICK_FREQ);


	InitializeLCD();

	clearScreen();

	uint8_t a;
//	for(a = 0; a < 50; ++a)
//	{
//		drawPixel(a, 14, 1);
//	}
//	drawLine(5, 0, 60, 20, 1);
//	drawRectangle(5, 1, 10, 5, 1, 1);



	uint8_t point, say; //silinecek
	uint8_t xr, yr;

	drawChar(0, 0, 'A', Font14);
//	drawChar(15, 15, 'G', Font14);

	drawText(20, 20, "HelloWorld!", Font14);

	while (1)
    {
//		DelayMS(200);
//		a = ++a%60;
//		drawLine(0, a, 3*a, 0, 0);
//		drawLine(0, a+1, 3*a+1, 0, 1);


		xr = rand() % 128;
		yr = rand() % 64;
//		drawPixel(xr, yr, 1);

		xr = rand() % 128;
		yr = rand() % 64;
//		drawPixel(xr, yr, 0);


		//SPI1_WriteByte(0x55);
//		DelayMS(1);
//		Console("say: ", ++say);

		LCDdrawFunction(); /* Sends changed pixels to LCD, in this function */
    }
}


/* main.c */
