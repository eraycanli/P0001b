/* lcd.h */
#pragma once

#include "LCD/fonts.h"


void InitializeLCD();
void SetLCDbrightness(uint16_t duty);


void drawPixel(uint8_t x, uint8_t y, uint8_t set);
void clearScreen();
void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t set);
void drawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t thickness, uint8_t set);
void drawFillRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t set);

void drawChar(uint8_t x, uint8_t y, char ch, FONT_INFO font);
void drawText(uint8_t x, uint8_t y, char *text, FONT_INFO font);




void LCDdrawFunction();



/* lcd.h */
