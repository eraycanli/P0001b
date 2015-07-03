/* commons.c */

#include "commons.h"

#ifdef DEBUG /* include only in debug mode */
	#include <stdio.h>
	#include <string.h>
#endif



volatile static uint16_t DelayTick;
uint32_t SysTickCount; /* If a device which works more than like 50 days, use uint64_t */



void DelayMS(uint16_t ms) /* max 65.535 seconds */
{
	DelayTick = ms;

	while(DelayTick); /* Wait until delay becomes zero */

	return;
}



/* Interrupt handler of core SysTick */
void SysTick_Handler(void)
{
	++SysTickCount; /* Counts System Time */

	if(DelayTick != 0)
	{
		--DelayTick; /* decrements delay time */
	}

	return;
}




/* Send console text from UART6 */
void Console(char string[], int32_t value)
{
#ifdef DEBUG

	uint16_t stringLength;
	uint16_t sendCounter = 0; /* Counts bytes of string*/
	stringLength = strlen(string); /* Determine length of the string */

	UART2_SendByte('-');
	UART2_SendByte('>');
	UART2_SendByte(' ');

	while(stringLength--) /* Send string here */
	{
		UART2_SendByte(string[sendCounter++]);
	}

	char valueString[16]; /* String for value, if exists */
	sprintf(valueString, "%d", value); /* Convert integer value to string */
	stringLength = strlen(valueString); /* Get length of value string */

	sendCounter = 0; /* Reset send counter */

	UART2_SendByte(' '); /* Send space before value */
	while(stringLength--) /* Send until finishes */
	{
		UART2_SendByte(valueString[sendCounter++]);
	}

	UART2_SendByte('\n'); /* Send carriage return (newline) */

#endif

	return;
}


/* commons.c */
