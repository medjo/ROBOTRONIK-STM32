#include "main.h"

#include <string.h>




long long int avg;
int avg_cnt=0;

void init_sensor1();
void activate_sensor1();

 /// Called from systick handler
 
void timing_handler() {
    if (time_var1) {
        time_var1--;
    }
    time_var2++;
}

 //Delay a number of systick cycles (1ms)
 
void Delay(volatile uint32_t nCount) {
    time_var1 = nCount;

    while(time_var1){};
}

void SysTick_Handler(void)
{
    timing_handler();
}


	// Timer handler declaration 
	TIM_HandleTypeDef        TimHandle;

	









// Timer Input Capture Configuration Structure declaration 
TIM_IC_InitTypeDef       sConfig;

// Slave configuration structure 
TIM_SlaveConfigTypeDef   sSlaveConfig;

// Captured Value 
__IO uint32_t            uwIC2Value = 0;
// Duty Cycle Value 
__IO long int            uwDutyCycle = 0;
// Frequency Value 
__IO uint32_t            uwFrequency = 0;


void TIM5_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle);

}

void Error_PWM_Input()
{
	while (1) {
        HAL_GPIO_TogglePin(GPIOD, LED_ROUGE);
        HAL_GPIO_TogglePin(GPIOD, LED_ORANGE);
        Delay(100);
    }
}



int main(void) {
	init();
	init_GPIO_LED();
	init_sensor1();
	//test_led();
	
	HAL_GPIO_TogglePin(GPIOD, LED_ORANGE);
    Delay(500);
    HAL_GPIO_TogglePin(GPIOD, LED_ORANGE);
	
	activate_sensor1();
	

	// Infinite loop 
	while (1)
	{
		Delay(500);
    HAL_GPIO_TogglePin(GPIOD, LED_VERTE);
	}

    return 0;
}


void activate_sensor1()
{
	//##-4- Start the Input Capture in interrupt mode ##########################
	if(HAL_TIM_IC_Start_IT(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
	{
	// Starting Error 
		Error_PWM_Input();
	}

	//##-5- Start the Input Capture in interrupt mode ##########################
	if(HAL_TIM_IC_Start_IT(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
	{
	// Starting Error 
		Error_PWM_Input();
	}
}


void init_sensor1()
{
		//PA0 : TIM5_CH1
	//PA1 : TIM5_CH2 PIN DE RECEPTION !!!
	//PA0 : TIM5_CH3
	//PA0 :	TIM5_CH4
	
	
	
	
	
	
	/// @brief TIM MSP Initialization 
	///        This function configures the hardware resources used in this example: 
	///           - Peripheral's clock enable
	///           - Peripheral's GPIO Configuration  
	/// @param htim: TIM handle pointer
	/// @retval None
	
	GPIO_InitTypeDef   GPIO_InitStruct;

	//##-1- Enable peripherals and GPIO Clocks #################################
	// TIMx Peripheral clock enable 
	__HAL_RCC_TIM5_CLK_ENABLE();

	// Enable GPIO channels Clock 
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure  (TIMx_Channel) in Alternate function, push-pull and 100MHz speed 
	GPIO_InitStruct.Pin = GPIO_PIN_1;//GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//##-2- Configure the NVIC for TIMx #########################################
	// Sets the priority grouping field 
	HAL_NVIC_SetPriority(TIM5_IRQn, 1, 1);

	// Enable the TIM5 global Interrupt 
	HAL_NVIC_EnableIRQ(TIM5_IRQn);
	
	
	
	
	
	
	
	
	
	//##-1- Configure the TIM peripheral ####################################### 
	// Set TIMx instance 
	TimHandle.Instance = TIM5;

	// Initialize TIMx peripheral as follow:
	//   + Period = 0xFFFF
	//   + Prescaler = 0
	//   + ClockDivision = 0
	//   + Counter direction = Up
	
	TimHandle.Init.Period = 0xFFFF;
	TimHandle.Init.Prescaler = 20;
	TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
	TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;  
	if(HAL_TIM_IC_Init(&TimHandle) != HAL_OK)
	{
	// Initialization Error 
		Error_PWM_Input();
	}

	//##-2- Configure the Input Capture channels ############################### 
	// Common configuration 
	sConfig.ICPrescaler = TIM_ICPSC_DIV1;
	sConfig.ICFilter = 0;  

	// Configure the Input Capture of channel 1 
	sConfig.ICPolarity = TIM_ICPOLARITY_FALLING;
	sConfig.ICSelection = TIM_ICSELECTION_INDIRECTTI;// TIM_ICSELECTION_DIRECTTI, TIM_ICSELECTION_INDIRECTTI or TIM_ICSELECTION_TRC
	if(HAL_TIM_IC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
	{
	// Configuration Error 
		Error_PWM_Input();
	}

	 //Configure the Input Capture of channel 2 
	sConfig.ICPolarity = TIM_ICPOLARITY_RISING;
	sConfig.ICSelection = TIM_ICSELECTION_DIRECTTI;
	if(HAL_TIM_IC_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
	{
		//Configuration Error 
		Error_PWM_Input();
	}
	
	
	//##-3- Configure the slave mode ###########################################
	// Select the slave Mode: Reset Mode 
	sSlaveConfig.SlaveMode     = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger  = TIM_TS_TI2FP2;
	if(HAL_TIM_SlaveConfigSynchronization(&TimHandle, &sSlaveConfig) != HAL_OK)
	{
	// Configuration Error 
		Error_PWM_Input();
	}

	
	
}




///
//  * @brief  Input Capture callback in non blocking mode 
//  * @param  htim: TIM IC handle
//  * @retval None

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    /* Get the Input Capture value */
    uwIC2Value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
    
    if (uwIC2Value != 0)
    {
      /* Duty cycle computation */
      uwDutyCycle = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);//)) / (uwIC2Value * 2);
      /*if (avg_cnt < 5)
      {
      	avg_cnt++;
      	avg += uwDutyCycle;
      } else
      {*/
		  //avg = avg / avg_cnt;
		  //avg_cnt = 0;
		  if(uwDutyCycle != 0){
		  
			  if (uwDutyCycle < 2500)
			  {
			  	HAL_GPIO_WritePin(GPIOD, LED_BLEUE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_ORANGE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_VERTE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_ROUGE, GPIO_PIN_SET);
			  } else if (uwDutyCycle < 8000)
			  {
			  	HAL_GPIO_WritePin(GPIOD, LED_BLEUE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_ORANGE, GPIO_PIN_SET);
			  	HAL_GPIO_WritePin(GPIOD, LED_VERTE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_ROUGE, GPIO_PIN_RESET);
			  } else if (uwDutyCycle < 15000)
			  {
			  	HAL_GPIO_WritePin(GPIOD, LED_BLEUE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_ORANGE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_VERTE, GPIO_PIN_SET);
			  	HAL_GPIO_WritePin(GPIOD, LED_ROUGE, GPIO_PIN_RESET);
			  } else
			  {
			  	HAL_GPIO_WritePin(GPIOD, LED_BLEUE, GPIO_PIN_SET);
			  	HAL_GPIO_WritePin(GPIOD, LED_ORANGE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_VERTE, GPIO_PIN_RESET);
			  	HAL_GPIO_WritePin(GPIOD, LED_ROUGE, GPIO_PIN_RESET);
			  }
		  }
		  avg = 0;
      //}
      
      /* uwFrequency computation
      TIM4 counter clock = (RCC_Clocks.HCLK_Frequency)/2 */      
      uwFrequency = (HAL_RCC_GetHCLKFreq())/2 / uwIC2Value;
    }
    else
    {
      uwDutyCycle = 0;
      uwFrequency = 0;
    }
  }
}

