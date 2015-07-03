/* IOexp.c */

#include "commons.h"
#include "BSP/IOexp.h"

#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_cortex.h"






I2C_HandleTypeDef handleI2C1; /* Create handle struct for I2C1 */


#define IOEX_ADDRESS 0x40 /* IO Expander device address */
	#define IOEX_READ 0x01 /* Should be added to device address, when reading registers */
	#define IOEX_WRITE 0x00 /* Should be added to device address, when writing to registers */

#define IOEX_AUTOINCREMENT 0x80 /* Should be added to command, when auto-increment will be used */

#define IOEX_IP0 0x00 /* Commands to read Input Port Registers */
#define IOEX_IP1 0x01
#define IOEX_IP2 0x02
#define IOEX_IP3 0x03
#define IOEX_IP4 0x04

#define IOEX_OP0 0x08 /* Commands to read/write Output Port Registers */
#define IOEX_OP1 0x09
#define IOEX_OP2 0x0A
#define IOEX_OP3 0x0B
#define IOEX_OP4 0x0C

#define IOEX_PI0 0x10 /* Commands to read/write Polarity Inversion Registers */
#define IOEX_PI1 0x11
#define IOEX_PI2 0x12
#define IOEX_PI3 0x13
#define IOEX_PI4 0x14

#define IOEX_IOC0 0x18 /* Commands to read/write I/O Configuration Registers */
#define IOEX_IOC1 0x19
#define IOEX_IOC2 0x1A
#define IOEX_IOC3 0x1B
#define IOEX_IOC4 0x1C

#define IOEX_MSK0 0x20 /* Commands to read/write Mask Interrupt Registers */
#define IOEX_MSK1 0x21
#define IOEX_MSK2 0x22
#define IOEX_MSK3 0x23
#define IOEX_MSK4 0x24



uint8_t IOEX_InputReg[2];
uint8_t IOEX_OutputReg[5];
uint8_t IOEX_ConfigReg[5];
uint8_t IOEX_MaskReg[5];






/* PCA9506DGG IO Expander works over I2C1
PB8: SCL
PB9: SDA
PC13: *INT (interrupt pin)
PC14: *OE pin (output enable)
PC15: *RESET pin
 */
void InitializeIOexpander()
{
	__GPIOB_CLK_ENABLE(); /* Enable I2C1 pins peripheral clock */

	__I2C1_CLK_ENABLE(); /* Enable I2C1 peripheral clock */

	GPIO_InitTypeDef GPIO_I2C1; /* Struct for I2C1 pins */
	GPIO_I2C1.Mode = GPIO_MODE_AF_OD; /* Alternate function, Open-Drain */
	GPIO_I2C1.Pin = GPIO_PIN_8 | GPIO_PIN_9; /* PB8:SCL PB9:SDA */
	GPIO_I2C1.Pull = GPIO_PULLUP; /* Pull up resistor for I2C */
	GPIO_I2C1.Speed = GPIO_SPEED_HIGH; /* High pin speed */
	GPIO_I2C1.Alternate = GPIO_AF4_I2C1; /* Pins connected to I2C1 */
	HAL_GPIO_Init(GPIOB, &GPIO_I2C1); /* Write pin config to registers */



	handleI2C1.Instance = I2C1; /* I2C1 is used */
	handleI2C1.Init.ClockSpeed = 400000; /* 400kHz bus speed */
	handleI2C1.Init.DutyCycle = I2C_DUTYCYCLE_2; /* 50:50 duty cycle */
	handleI2C1.Init.OwnAddress1 = 0x00; /* Address for this device. Not necessary */
	handleI2C1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT; /* 7 bit address mode */
	handleI2C1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED; /* Single address is enough */
	handleI2C1.Init.OwnAddress2 = 0x00; /* Not important since dual address mode is disabled */
	handleI2C1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED; /* General call disabled, since this device is not slave */
	handleI2C1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED; /* Stretch mode is free, slave can stretch the bus */
	Console("I2C1 HAL_I2C_Init (0 is ok):", HAL_I2C_Init(&handleI2C1)); /* Write configuration to I2C register */



	__GPIOC_CLK_ENABLE(); /* Enable C Port peripheral clock */

	GPIO_InitTypeDef GPIO_INT; /* Create a struct for INTerrupt pin */
	GPIO_INT.Pin = GPIO_PIN_13; /* PC13 */
	GPIO_INT.Mode = GPIO_MODE_IT_RISING_FALLING; /* INTerrupt on both edges */
	GPIO_INT.Pull = GPIO_PULLUP; /* Pull up resistor */
	HAL_GPIO_Init(GPIOC, &GPIO_INT); /* Write to PORTC register */

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0); /* Pre group 1, sub 2 */
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn); /* Enable External interrupt on PC13 */.



	GPIO_InitTypeDef GPIO_IOpins; /* Create a struct for other IO pins */

	GPIO_IOpins.Pin = GPIO_PIN_14 | GPIO_PIN_15; /* PC14: *OE PC15: *RESET */
	GPIO_IOpins.Mode = GPIO_MODE_OUTPUT_PP; /* Output Push-Pull */
	GPIO_IOpins.Pull = GPIO_NOPULL; /* No pull resistors */
	GPIO_IOpins.Speed = GPIO_SPEED_HIGH; /* Low speed */
	HAL_GPIO_Init(GPIOC, &GPIO_IOpins); /* Initialize IO port registers */


	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET); /* Reset pin active (LOW) */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET); /* Enable Output of IOexp. */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET); /* Reset pin not active (HIGH) */





	IOEX_ConfigReg[0] = 0b11111111; /* Port 1 pins are all input */
	IOEX_ConfigReg[1] = 0b00000111; /* Last 3 bits of Port 2 are input */
	IOEX_ConfigReg[2] = 0b00000000; /* All output */
	IOEX_ConfigReg[3] = 0b00000000; /* All output */
	IOEX_ConfigReg[4] = 0b00000000; /* All output */
	/* Write all to IO Expander Registers */
	HAL_I2C_Mem_Write(&handleI2C1, IOEX_ADDRESS | IOEX_WRITE, IOEX_IOC0, I2C_MEMADD_SIZE_8BIT, &IOEX_ConfigReg[0], 5, 5000);


	IOEX_OutputReg[0] = 0b00000000; /* Make all output registers zero first */
	IOEX_OutputReg[1] = 0b00000000;
	IOEX_OutputReg[2] = 0b00000000;
	IOEX_OutputReg[3] = 0b00000000;
	IOEX_OutputReg[4] = 0b00000000;
	/* Write all to IO Expander Registers */
	HAL_I2C_Mem_Write(&handleI2C1, IOEX_ADDRESS | IOEX_WRITE, IOEX_OP0, I2C_MEMADD_SIZE_8BIT, &IOEX_OutputReg[0], 5, 5000);


	/* Read input ports initially */
	HAL_I2C_Mem_Read(&handleI2C1, IOEX_ADDRESS | IOEX_READ, IOEX_IP0, I2C_MEMADD_SIZE_8BIT, &IOEX_InputReg[0], 2, 5000);


	IOEX_MaskReg[0] = 0b00000000; /* Interrupt enabled */
	IOEX_MaskReg[1] = 0b11111000; /* Interrupt enabled on last 3 input bits */
	IOEX_MaskReg[2] = 0b11111111; /* Interrupt disable */
	IOEX_MaskReg[3] = 0b11111111; /* Interrupt disable */
	IOEX_MaskReg[4] = 0b11111111; /* Interrupt disable */
	/* Write all to IO Expander Registers */
	HAL_I2C_Mem_Write(&handleI2C1, IOEX_ADDRESS | IOEX_WRITE, IOEX_MSK0, I2C_MEMADD_SIZE_8BIT, &IOEX_MaskReg[0], 5, 5000);


	return;
}



/* Set given output pin to status 1/0 */
void SetIOEXoutput(uint8_t pin, uint8_t status)
{
	if(pin < 11 || pin > 39) /* From 11 to 39 only */
	{
		return;
	}

	uint8_t bank = pin / 8; /* which bank */
	uint8_t port = pin % 8; /* which port/bit */

	if(status) /* Set output */
	{
		IOEX_OutputReg[bank] |= 0x01 << port; /* Put 1 to corresponding bit */
	}
	else /* Clear output */
	{
		IOEX_OutputReg[bank] &= ~(0x01 << port); /* Clear corresponding bit */
	}

	/* Update corresponding register */
	HAL_I2C_Mem_Write(&handleI2C1, IOEX_ADDRESS | IOEX_WRITE, IOEX_OP0, I2C_MEMADD_SIZE_8BIT, &IOEX_OutputReg[bank], 1, 5000);

	return;
}






#define BUTTON_NUMBER 11

uint8_t ButtonFlag[BUTTON_NUMBER]; /* Flag for 11 buttons. Set to 1 if pressed */

uint16_t ButtonLastState; /* Keeps last state of buttons, before second interrupt */

/* Should be placed in IO Expander Interrupt handler */
void ButtonInterruptHandler()
{
	/* Read 2 input registers from IO Expander */
	HAL_I2C_Mem_Read(&handleI2C1, IOEX_ADDRESS | IOEX_READ, IOEX_IP0, I2C_MEMADD_SIZE_8BIT, &IOEX_InputReg[0], 2, 5000);


	uint16_t buttonState; /* current state of button registers */
	buttonState = IOEX_InputReg[1] << 8 | IOEX_InputReg[0]; /* Combine two registers */


	uint8_t i;
	for(i = 0; i < BUTTON_NUMBER; ++i) /* Iterate for every bit/button */
	{
		if(!(ButtonLastState & (0x0001 << i)) && (buttonState & (0x0001 << i))) /* Last interrupt pressed, now released */
		{
			ButtonFlag[i] = 1; /* Set button pressed flag */
		}
	}


	ButtonLastState = buttonState; /* Configure this as last state */

	return;
}





void EXTI15_10_IRQHandler(void)
{
	ButtonInterruptHandler();

	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);

	return;
}








/* IOexp.c */
