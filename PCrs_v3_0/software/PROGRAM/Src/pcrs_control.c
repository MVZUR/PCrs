/*
 * pcrs_control.c
 *
 *  Created on: Sep 5, 2025
 *      Author: mmazur
 */


#include "main.h"

// system status variable
volatile sys_statuses system_status = STATUS_SLEEPING;		// initial value is STATUS_SLEEP to begin power on procedure after turning the key in the ignition

// timer variable
extern TIM_HandleTypeDef htim2;


// ******************************************************************************************
// *               		DISABLING / ENABLING PERIPHERALS CLOCKS / INTERRUPTS		        *
// ******************************************************************************************

// turn on/off GPIO module
void control_GPIO_enable() {
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
}
void control_GPIO_disable() {

	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
}


// turn on/off ADC1 module
void control_ADC1_enable() {
	__HAL_RCC_ADC1_CLK_ENABLE();
}
void control_ADC1_disable() {
	__HAL_RCC_ADC1_CLK_DISABLE();
}


// turn on/off I2C1 FM+ module
/*void control_FMPI2C1_enable() {
	  HAL_FMPI2CEx_EnableFastModePlus(FMPI2C_FASTMODEPLUS_SCL);
	  HAL_FMPI2CEx_EnableFastModePlus(FMPI2C_FASTMODEPLUS_SDA);
}
void control_FMPI2C1_disable() {
	  HAL_FMPI2CEx_DisableFastModePlus(FMPI2C_FASTMODEPLUS_SCL);
	  HAL_FMPI2CEx_DisableFastModePlus(FMPI2C_FASTMODEPLUS_SDA);
}*/


// turn on/off I2C1 module
void control_I2C1_enable() {
	__HAL_RCC_I2C1_CLK_ENABLE();
}
void control_I2C1_disable() {
	__HAL_RCC_I2C1_CLK_DISABLE();
}
// turn on/off I2C2 module
void control_I2C2_enable() {
	__HAL_RCC_I2C2_CLK_ENABLE();
}
void control_I2C2_disable() {
	__HAL_RCC_I2C2_CLK_DISABLE();
}
// turn on/off I2C3 module
void control_I2C3_enable() {
	__HAL_RCC_I2C3_CLK_ENABLE();
}
void control_I2C3_disable() {
	__HAL_RCC_I2C3_CLK_DISABLE();
}


// turn on/off SPI1 module
void control_SPI1_enable() {
	__HAL_RCC_SPI1_CLK_ENABLE();
}
void control_SPI1_disable() {
	__HAL_RCC_SPI1_CLK_DISABLE();
}
// turn on/off SPI2 module
void control_SPI2_enable() {
	__HAL_RCC_SPI2_CLK_ENABLE();
}
void control_SPI2_disable() {
	__HAL_RCC_SPI2_CLK_DISABLE();
}
// turn on/off SPI3 module
/*void control_SPI3_enable() {
	__HAL_RCC_SPI3_CLK_ENABLE();
}
void control_SPI3_disable() {
	__HAL_RCC_SPI3_CLK_DISABLE();
}*/


// turn on/off TIM2 input capture interrupt
void control_TIM2_it_enable(void) {
    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}
void control_TIM2_it_disable(void) {
    HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);
}
// turn on/off TIM2 module
void control_TIM2_enable() {
	__HAL_RCC_TIM2_CLK_ENABLE();
}
void control_TIM2_disable() {
	__HAL_RCC_TIM2_CLK_DISABLE();
}

// ******************************************************************************************
