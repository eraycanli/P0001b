/* lcdBSP.c */

#include "commons.h"

#include "BSP/lcdBSP.h"

#include "BSP/spi.h"

#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_tim.h"




TIM_HandleTypeDef htim1; /* Create struct for TMR1 base init. */
TIM_OC_InitTypeDef sConfig1; /* Create struct for PWM OC init. */



/* Initializes SPI1 and PB4 (ChipSelect), PE4 (A0 command), PE6 (RST) and PE11 (SHDN/Brightness) pins */
void InitializeLCDpins()
{
	InitializeSPI1(); /* SPI1 is used for LCD control */


	__GPIOB_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();


	GPIO_InitTypeDef GPIO_ChipSelect; /* Create struct for SPI CS pin */
	GPIO_ChipSelect.Mode = GPIO_MODE_OUTPUT_PP; /* Output push-pull */
	GPIO_ChipSelect.Pin = GPIO_PIN_4; /* PB4 */
	GPIO_ChipSelect.Pull = GPIO_NOPULL; /* No resistor */
	GPIO_ChipSelect.Speed = GPIO_SPEED_HIGH; /* High speed */
	HAL_GPIO_Init(GPIOB, &GPIO_ChipSelect); /* Write configuration to register */
	Console("GPIO Init PB4 CS", 1);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET); /* Start as de-selected */



	GPIO_InitTypeDef GPIO_A0; /* Create struct for A0 command pin */
	GPIO_A0.Mode = GPIO_MODE_OUTPUT_PP; /* Output push-pull */
	GPIO_A0.Pin = GPIO_PIN_4; /* PE4 */
	GPIO_A0.Pull = GPIO_NOPULL; /* No resistor */
	GPIO_A0.Speed = GPIO_SPEED_HIGH; /* High speed */
	HAL_GPIO_Init(GPIOE, &GPIO_A0); /* Write configuration to register */
	Console("GPIO Init PE4 A0", 1);



	GPIO_InitTypeDef GPIO_RST; /* Create struct for RESET pin */
	GPIO_RST.Mode = GPIO_MODE_OUTPUT_PP; /* Output push-pull */
	GPIO_RST.Pin = GPIO_PIN_6; /* PE6 */
	GPIO_RST.Pull = GPIO_NOPULL; /* No resistor */
	GPIO_RST.Speed = GPIO_SPEED_HIGH; /* High speed */
	HAL_GPIO_Init(GPIOE, &GPIO_RST); /* Write configuration to register */
	Console("GPIO Initted PE6 RST", 1);

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET); /* Set to high, NOT reset */



	GPIO_InitTypeDef GPIO_PWM; /* Create struct for PWM GPIO pin */
	GPIO_PWM.Pin = GPIO_PIN_11; /* PE11 */
	GPIO_PWM.Pull = GPIO_NOPULL; /* No resistor */
	GPIO_PWM.Mode = GPIO_MODE_AF_PP; /* Alternate function mode */
	GPIO_PWM.Speed = GPIO_SPEED_HIGH; /* High speed */
	GPIO_PWM.Alternate = GPIO_AF1_TIM1; /* Pins connected to TIM1 */
	HAL_GPIO_Init(GPIOE, &GPIO_PWM); /* Initialize GPIO pins with config */
	Console("GPIO Initted PE11 PWM", 1);

	__TIM1_CLK_ENABLE(); /* Enable TMR1 peripheral clock */

	htim1.Instance = TIM1; /* Timer1 selected */
	htim1.Init.Prescaler = (SystemCoreClock/2) / 1000000 - 1; /* 1MHz timer clock */
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP; /* counts zero to period */
	htim1.Init.Period = 1000; /* PWM period */
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; /* zero clock division */
	HAL_TIM_PWM_Init(&htim1); /* Initialize TMR1 with this config in PWM mode */
	Console("TIM1 Initted", 1);

	sConfig1.OCMode = TIM_OCMODE_PWM1;
	sConfig1.Pulse = 0; /* Start as zero */
	sConfig1.OCPolarity = TIM_OCPOLARITY_HIGH; /* High when active (duty) */
	sConfig1.OCFastMode = TIM_OCFAST_ENABLE; /* Fast */

	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfig1, TIM_CHANNEL_2); /* Configure channel 2 */

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); /* Start channel 2 PWM */

	TIM1->CCR2 = 0; /* Set channel 2 duty cycle to zero */


	return;
}





#define SELECT GPIO_PIN_RESET
#define DESELECT GPIO_PIN_SET
static void LCDchipSelect(uint8_t select)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, select);

	return;
}





#define A0_DATA 1
#define A0_COMMAND 0
static void LCDA0pin(uint8_t data) /* Set A0 pin to DATA(1) or COMMAND(0) */
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, data);

	return;
}





void LCDsendCommand(uint8_t command)
{
	LCDA0pin(A0_COMMAND); /* Set A0 pin to COMMAND */

	LCDchipSelect(SELECT);
	SPI1_WriteByte(command);
	LCDchipSelect(DESELECT);

	return;
}




void LCDsendData(uint8_t data)
{
	LCDA0pin(A0_DATA); /* Set A0 pin to DATA */

	LCDchipSelect(SELECT);
	SPI1_WriteByte(data);
	LCDchipSelect(DESELECT);

	return;
}








/* lcdBSP.c */
