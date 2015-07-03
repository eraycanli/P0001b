/* uart.c */

#include "commons.h"

#include "stm32f4xx_hal_uart.h"

#include "BSP/uart.h"




UART_HandleTypeDef huart2; /* Create a struct for USART2 */

void InitializeUsart2()
{
	__GPIOA_CLK_ENABLE(); /* Enable GPIOA clock */

	__USART2_CLK_ENABLE(); /* Enable peripheral clock */

	GPIO_InitTypeDef GPIO_InitStruct; /* Create a struct */
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3; /* PA2 & PA3 */
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; /* alternate function mode */
	GPIO_InitStruct.Pull = GPIO_PULLUP; /* pull up resistor */
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH; /* Medium speed */
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2; /* USART2 AF */
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); /* Write to registers */

	HAL_NVIC_SetPriority(USART2_IRQn, 0, 1); /* priority 1 1 */
	HAL_NVIC_EnableIRQ(USART2_IRQn); /* Enable int. */

	huart2.Instance = USART2; /* Select peripheral */
	huart2.Init.BaudRate = 500000;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX; /* TxRx mode */
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart2);

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

	Console("UART2 Status:", HAL_UART_GetState(&huart2)); /* Gets UART2 status */
	Console("UART2 Baud Rate:", huart2.Init.BaudRate);

	return;
}



/* Sends 1 byte and returns HAL Status (HAL_OK is 0x00) */
uint8_t UART2_SendByte(uint8_t data)
{
	return HAL_UART_Transmit(&huart2, &data, 1, 5000); /* Send and get returning code */
}



/* uart.c */
