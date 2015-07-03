/* lcd.c */



#include "commons.h"

#include "LCD/lcd.h"

#include "BSP/lcdBSP.h"

//#include "LCD/fonts.h"

#include <math.h>







#define LCD_WIDTH 128
#define LCD_HEIGHT 64


#define PAGE_NUMBER 8
#define PAGE_SIZE 128


uint8_t dataBuffer[LCD_WIDTH * LCD_HEIGHT / 8]; /* Create an array simulating the RAM inside STR7565 controller */

uint8_t dirtyPages[PAGE_NUMBER]; /* Flag variable for pages to update */







/* (x,y) pixel location
set: set/clear the pixel */
void drawPixel(uint8_t x, uint8_t y, uint8_t set)
{
	if((x >= LCD_WIDTH) || (y >= LCD_HEIGHT)) /* If the pixel is out of LCD, do not set it */
	{
		return;
	}


	uint16_t arrayPos = x + ((y / 8) * 128); /* Determines the byte in the array */
	uint8_t currentByte = dataBuffer[arrayPos]; /* Gets current value of the byte in array */


	if(set) /* Set the pixel bit */
	{
		dataBuffer[arrayPos] |= ((0x01) << (y%8)); /* Add 1 bit to corresponding position of byte */
	}
	else /* Remove the pixel bit */
	{
		dataBuffer[arrayPos] &= ~((0x01) << (y%8)); /* Make the corresponding bit zero in byte */
	}


	if(dataBuffer[arrayPos] != currentByte) /* Value of byte has changed, we will commit a dirty page */
	{
		dirtyPages[arrayPos / PAGE_SIZE] = 1; /* Set the corresponding page as 'dirty' */
	}

	return;
}





/* Clears whole screen */
void clearScreen()
{
	uint16_t i;

	for(i = 0; i < (LCD_WIDTH * LCD_HEIGHT / 8); ++i)
	{
		dataBuffer[i] = 0; /* Reset all elements in array */
		/* may use 'memset' to clear array */

		if((i % PAGE_SIZE) == 0) /* Page start */
		{
			LCDsendCommand(PAGE_ADDRESS | (i / PAGE_SIZE)); /* Send new page address */
			LCDsendCommand(COLUMN_ADDRESS); /* Send 4bit MSB of column */
			LCDsendCommand(0x00); /* Send 4bit LSB of column */
		}

		LCDsendData(0); /* Send zero value to RAM */
	}

	return;
}





/* (x0, y0) start point
(x1, y1) end point
set: set/clear pixels */
void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t set)
{
	uint8_t steep = (abs(y1-y0) > abs(x1-x0)); /* Line is steeper than 45 degrees */

	uint8_t swapA, swapB;

	if(steep)
	{
		swapA = x0; /* swap x0 and y0 */
		swapB = y0;
		x0 = swapB;
		y0 = swapA;

		swapA = x1; /* swap x1 and y1 */
		swapB = y1;
		x1 = swapB;
		y1 = swapA;
	}

	if(x0 > x1)
	{
		swapA = x0; /* swap x0 and x1 */
		swapB = x1;
		x0 = swapB;
		x1 = swapA;

		swapA = y0; /* swap y0 and y1 */
		swapB = y1;
		y0 = swapB;
		y1 = swapA;
	}


	uint8_t dx = x1 - x0; /* Delta X */
	uint8_t dy = abs(y1 - y0); /* Delta Y */


	int8_t err = dx / 2;
	int8_t ystep;


	if(y0 < y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}


	for(; x0<x1; x0++)
	{
		if(steep)
		{
			drawPixel(y0, x0, set);
		}
		else
		{
			drawPixel(x0, y0, set);
		}

		err -= dy;

		if(err < 0)
		{
			y0 += ystep;
			err += dx;
		}
	}

	return;
}





/* (x,y) is left-top point
w: width
h: height
thickness: border thickness
set: set/clear pixels */
void drawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t thickness, uint8_t set)
{
	if(thickness >= w/2 || thickness >= h/2) /* If thickness is greater than width/2 or height/2 */
	{
		return;
	}


	uint8_t t, i, j;

	for(t = 0; t < thickness; ++t) /* Thick border */
	{
		for(i = x+t; i < x + w - 2*t; ++i) /* Draw horizontal lines */
		{
			drawPixel(i, y+t, set); /* Upper line */
			drawPixel(i, y+h-t, set); /* Lower line */
		}

		for(j = y+t; j < y + h - 2*t; ++j) /* Draw vertical lines */
		{
			drawPixel(x+t, j, set); /* Draw left lines */
			drawPixel(x+w-t, j, set); /* Draw right lines */
		}
	}

	return;
}





/* (x,y) is left-top point
w: width
h: height
set: set/clear the pixels */
void drawFillRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t set)
{
	uint8_t i, j;

	for(i = y; i < y + h; ++i) /* Line positions in y-axis */
	{
		for(j = x; j < x + w; ++j) /* 1 horizontal line */
		{
			drawPixel(j, i, set);
		}
	}

	return;
}





/* Returns width of given string, in selected font */
uint8_t getTextWidth(char *text, FONT_INFO font)
{
	uint8_t width; /* Keeps width by summing */
	char ch; /* Var for every char in string */
	FONT_CHAR_INFO character;


	uint8_t counter = 0;

//	while(*(text + counter)) // try this also
	while((ch = *(text + counter))) /* While still chars available in string */
	{
		character = *(font.descriptor + (ch - font.startChar));

		width += character.width; /* Add width of character to variable */
		width += font.spaceWidth; /* Add space width after characters */

		++counter;
	}


	return (width - font.spaceWidth); /* Remove spaceWidth amount, after last character */
}




/* Returns character height of given font */
uint8_t getTextHeight(FONT_INFO font)
{
	return font.height;
}



/* Draw a single char at given x,y */
void drawChar(uint8_t x, uint8_t y, char ch, FONT_INFO font)
{
	uint8_t i, j;

	FONT_CHAR_INFO character = *(font.descriptor + (ch - font.startChar)); /* Get the current char descriptor values */

	uint16_t currentLine;
	uint8_t dummyByte;

	for(i = 0; i < character.width; ++i) /* iterate every Line */
	{
		dummyByte = *((font.bitmap + character.offset) + 2*i); /* get Value of first byte */
		currentLine = *((font.bitmap + character.offset) + 2*i + 1); /* get Value of second byte */

		currentLine |= dummyByte<<8; /* Combine two bytes */

		for(j = 0; j < character.height; ++j) /* iterate every 'active' bit in line */
		{
			if(currentLine & (0x0001 << (15-j))) /* Current bit exists, put a pixel */
			{
				drawPixel(x + i, y + (font.height - j), 1); /* Put a pixel to corresponding point */
			}
		}
	}

	return;
}



void drawText(uint8_t x, uint8_t y, char *text, FONT_INFO font)
{
	// We can clean background here, optionally
	// drawRectangle(x, y, getTextWidth(text, font), getTextHeight(font), 0); // Clear background

	char ch;

	FONT_CHAR_INFO character;

	uint8_t xOffset = x;

	uint8_t counter = 0;

	while(ch = *(text + counter)) /* Get the current character and continue while it exists */
	{
		character = *(font.descriptor + (ch - font.startChar));

		drawChar(xOffset, y, ch, font); /* Put char */

		xOffset += (character.width + font.spaceWidth); /* Move x offset to right, for next character */

		++counter;
	}

	return;
}






/* Initializes LCD Display */
void InitializeLCD()
{
	InitializeLCDpins(); /* Configure SPI and pins related to LCD */


	/* Send LCD initialization commands here */
	LCDsendCommand(START_LINE | 0x00); /* Display start line 0 */

	LCDsendCommand(ADC_REVERSE);

	LCDsendCommand(COM_OUTPUT_NORMAL); /* Normal COM0~C0M63 */

	LCDsendCommand(DISPLAY_NORMAL);

	LCDsendCommand(LCD_BIAS_1o9); /* Bias set to 1/9 (1/65 Duty) */

	LCDsendCommand(POWER_MODE | BOOSTER_ON | VOLTAGE_REGULATOR_ON | VOLTAGE_FOLLOWER_ON); /* Set power mode as booster, voltage regulator and follower ON */

	LCDsendCommand(BOOSTER_RATIO); /* Booster ratio set command */
	LCDsendCommand(BOOSTER_4X); /* Booster ratio sent after its command */

	LCDsendCommand(RESISTOR_RATIO | 0x07); /* Voltage regulator set */

	LCDsendCommand(VOLUME_SET); /* Contrast set command */
	LCDsendCommand(0x10); /* Setting is sent just after command */

	LCDsendCommand(STATIC_INDICATOR_OFF); /* Static indicator set command */
	LCDsendCommand(0x00); /* Static indicator mode sent just after command */

	//LCDsendCommand(ALL_POINTS_ON); /* Make all points on */

	LCDsendCommand(DISPLAY_ON); /* Display turned ON */


	return;
}





/* Set LCD brightness PWM duty (0-1000) */
void SetLCDbrightness(uint16_t duty)
{
	TIM1->CCR2 = duty;

	return;
}





uint8_t p, b; /* Iteration for page update and byte sending */
/* This function draws changed pixels. Should be called in while(1) loop */
void LCDdrawFunction()
{
	for(p = 0; p < PAGE_NUMBER; ++p)
	{
		if(dirtyPages[p] == 1)
		{
			dirtyPages[p] = 0; /* Reset dirtyPage flag, at the beginning */

			LCDsendCommand(PAGE_ADDRESS | p); /* Send new page address */
			LCDsendCommand(COLUMN_ADDRESS); /* Send 4bit MSB of column */
			LCDsendCommand(0x00); /* Send 4bit LSB of column */

			for(b = 0; b < PAGE_SIZE; ++b)
			{
				LCDsendData(dataBuffer[p * PAGE_SIZE + b]); /* Send corresponding 128 bytes from dataBuffer */
			}
		}
	}

	return;
}





/* lcd.c */
