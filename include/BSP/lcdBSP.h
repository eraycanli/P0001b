/* lcdBSP.h */
#pragma once





#define DISPLAY_ON 0xAF
#define DISPLAY_OFF 0xAE


#define START_LINE 0x40 /* Add desired start line to last 6 bits of this command */


#define PAGE_ADDRESS 0xB0 /* Add page address to last 4 bits of this command */
#define COLUMN_ADDRESS 0x10 /* Add MSB 4bits of column address to last 4 bits of this command,
the next command after this must be LSB 4bits of the column address */


#define ADC_NORMAL 0xA0
#define ADC_REVERSE 0xA1


#define DISPLAY_NORMAL 0xA6
#define DISPLAY_REVERSE 0xA7


#define ALL_POINTS_OFF 0xA4
#define ALL_POINTS_ON 0xA5


#define LCD_BIAS_1o9 0xA2
#define LCD_BIAS_1o7 0xA3


#define RESET 0xE2


#define COM_OUTPUT_NORMAL 0xC0
#define COM_OUTPUT_REVERSE 0xC8


#define POWER_MODE 0x28 /* Add next bits to this power mode command */
	#define BOOSTER_ON 0x04 /* D2: Booster circuit control bit */
	#define VOLTAGE_REGULATOR_ON 0x02 /* D1: Voltage regulator circuit control bit */
	#define VOLTAGE_FOLLOWER_ON 0x01 /* D0: Voltage follower circuit control bit */


#define RESISTOR_RATIO 0x20 /* Add voltage regulator internal resistor ratio (Rb/Ra) to last 3 bits of this command */


#define VOLUME_SET 0x81 /* After this command, a byte must be send with last 6bits as Vo output voltage electronic volume register */


#define STATIC_INDICATOR_OFF 0xAC /* After static indicator commands, a byte must be set with last 2 bytes as flashing mode */
#define STATIC_INDICATOR_ON 0xAD


#define BOOSTER_RATIO 0xF8 /* After this, booster ratio value command must be sent*/
	#define BOOSTER_4X 0x00
	#define BOOSTER_5X 0x01
	#define BOOSTER_6x 0x03






void InitializeLCDpins();

void LCDsendCommand(uint8_t command);
void LCDsendData(uint8_t data);








/* lcdBSP.h */

