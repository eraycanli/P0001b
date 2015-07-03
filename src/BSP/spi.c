/* spi.c */

#include "commons.h"

#include "BSP/spi.h"
#include "stm32f4xx_hal_spi.h"


SPI_HandleTypeDef  hspi1; /* Create a handle struct for SPI */

void InitializeSPI1()
{
	__SPI1_CLK_ENABLE(); /* Enable peripheral clock */
	__GPIOB_CLK_ENABLE(); /* Enable SPI pins peripheral clock */

	GPIO_InitTypeDef GPIO_InitStruct; /* Create a struct for SPI pins configuration */
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; /* Alternate Function Push-Pull Mode */
	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5; /* PB3:SCLK PB5:MOSI */
	GPIO_InitStruct.Pull = GPIO_NOPULL; /* No pull */
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH; /* High speed */
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1; /* Connect pins to SPI1 */
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); /* Write configuration to pins */

	hspi1.Instance = SPI1; /* SPI1 is the peripheral */
	hspi1.Init.Mode = SPI_MODE_MASTER; /* MCU is the master */
	hspi1.Init.Direction = SPI_DIRECTION_1LINE; /* 1 line communication MOSI */
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT; /* 8 bit data size */
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH; /* State HIGH when idle */
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE; /* Capture (or send) data on second (rising) edge */
	hspi1.Init.NSS = SPI_NSS_SOFT; /* Chip select done by software */
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; /* fPCLK1(42MHz) / 2 baud */
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB; /* MSB first */
	hspi1.Init.TIMode = SPI_TIMODE_DISABLED; /* TI Mode not used when master */
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED; /* We are not using CRC for this device */
	hspi1.Init.CRCPolynomial = 7; /* Reset value is 7. Do not change! */
	Console("SPI1 HAL_SPI_Init (0 is ok):", HAL_SPI_Init(&hspi1)); /* Write values to SPI register */




	return;
}




/* Writes 1 byte of data with SPI1 */
uint8_t SPI1_WriteByte(uint8_t data)
{
//	while(hspi1.Instance->SR & (uint16_t)0x0080); /* While Busy flag is set */
//	hspi1.Instance->DR= data;
//	SPI1->DR = data; /* This is faster */
	return HAL_SPI_Transmit(&hspi1, &data, 1, 5000); /* Write a single byte */
//	return 1;
}






/* spi.c */
