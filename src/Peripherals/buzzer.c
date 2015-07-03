/* buzzer.c */


#include "commons.h"

#include "stm32f4xx_hal_tim.h"

#include "Peripherals/buzzer.h"


#define PWMPERIOD 1000

/* Initializes Buzzer PWM pin (PE9) */
void InitializeBuzzer()
{
	__GPIOE_CLK_ENABLE(); /* Enable GPIOE clock */

	TIM_HandleTypeDef htim1; /* Create struct for TMR1 base init. */

	GPIO_InitTypeDef GPIO_BuzzerPWM; /* Create struct for GPIO config */
	GPIO_BuzzerPWM.Pin = GPIO_PIN_9; /* PE9 */
	GPIO_BuzzerPWM.Mode = GPIO_MODE_AF_PP; /* Alternate function mode */
	GPIO_BuzzerPWM.Pull = GPIO_NOPULL; /* no push or pull resistors */
	GPIO_BuzzerPWM.Speed = GPIO_SPEED_HIGH; /* High speed */
	GPIO_BuzzerPWM.Alternate = GPIO_AF1_TIM1; /* Pin connected to TIM1 */
	HAL_GPIO_Init(GPIOE, &GPIO_BuzzerPWM); /* Initialize GPIO pin with the configuration */


	__TIM1_CLK_ENABLE(); /* Enable Timer1 peripheral clock */

	htim1.Instance = TIM1; /* Timer1 selected */
	htim1.Init.Prescaler = (SystemCoreClock/2) / 1000000 - 1; /* 1MHz timer clock */
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP; /* counts zero to period */
	htim1.Init.Period = PWMPERIOD; /* PWM duty period. 1MHz/1000 = 1kHz PWM output generated */
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; /* zero clock division */
	Console("TMR1 HAL_TIM_PWM_Init (0 ok):", HAL_TIM_PWM_Init(&htim1)); /* Initialize TMR3 with this config in PWM mode */


	TIM_OC_InitTypeDef OCstruct_PWM; /* Create struct for PWM OC init. */
	OCstruct_PWM.OCMode = TIM_OCMODE_PWM1; /* Dont know what it is */
	OCstruct_PWM.Pulse = 0; /* Start as zero */
	OCstruct_PWM.OCPolarity = TIM_OCPOLARITY_HIGH; /* High when active (duty) */
	OCstruct_PWM.OCFastMode = TIM_OCFAST_ENABLE; /* Fast */

	HAL_TIM_PWM_ConfigChannel(&htim1, &OCstruct_PWM, TIM_CHANNEL_1); /* Configure PWM channel 1 */


	TIM1->CCR1 = 0; /* Set PWM duty to zero */
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); /* Enable PWM output */

	return;
}





static uint8_t BeepCount; /* Number of set beeps */
static uint16_t BeepLength, BeepDelay; /* Beeping and delaying lengths */

#define IDLE 0
#define BEEPING 1
#define DELAYING 2
static uint8_t BeepState; /* 0: IDLE, 1: BEEPING, 2: DELAYING */
static int16_t BeepingCounter, BeepDelayCounter; /* Keeps timing while beeping and delaying */

/* Buzzer Beep Function.
beepCount: # of beeps
length: 1 beep length (in Ms)
delayAfter: delay between beeps (in Ms) */
void BuzzerBeep(uint8_t beepCount, uint16_t length, uint16_t delayAfter)
{
	BeepLength = length;
	BeepDelay = delayAfter;
	BeepCount = beepCount;

	BeepState = IDLE; /* Cancels, in any previous beep exists */

	return;
}






/* Control Buzzer PWM ON/OFF switch in interrupt, without blocking the program
Should be placed in 100Hz timer interrupt */
void BuzzerControlCycle()
{
	if(BeepCount > 0) /* If beep count exists */
	{
		switch(BeepState)
		{
			case IDLE: /* Not beeping, Start it */
				TIM1->CCR1 = PWMPERIOD/2; /* Set PWM output */
				BeepState = BEEPING; /* Set state to beeping */
				BeepingCounter = BeepLength; /* Set its length */
				break;


			case BEEPING: /* Beeping */
				BeepingCounter -= 10; /* Decrement beeping counter */
				if(BeepingCounter < 0) /* Beeping has ended */
				{
					TIM1->CCR1 = 0; /* Cancel PWM output */
					--BeepCount; /* Decrease beep counts */

					if(BeepCount == 0) /* This was the last beep, take state to IDLE */
					{
						BeepState = IDLE;
					}
					else  /* More beeps exist */
					{
						BeepState = DELAYING; /* Set state to delaying */
						BeepDelayCounter = BeepDelay; /* Set delaying duration */
					}
				}
				break;


			case DELAYING: /* In delay */
				BeepDelayCounter -= 10; /* Decrement delay counter */
				if(BeepDelayCounter < 0) /* Delay ended */
				{
					TIM1->CCR1 = PWMPERIOD/2; /* Give PWM output */
					BeepState = BEEPING; /* Set state to beeping */
					BeepingCounter = BeepLength; /* Set beeping duration */
				}
				break;
		}
	}

	return;
}




/* buzzer.c */
