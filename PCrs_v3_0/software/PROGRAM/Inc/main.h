/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "pcrs_control.h"
#include "pcrs_tasks.h"
#include "pcrs_menu.h"
#include "pcrs_delay.h"
#include "pcrs_display.h"
#include "pcrs_clock.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO_EXTI0_idle_Pin GPIO_PIN_0
#define GPIO_EXTI0_idle_GPIO_Port GPIOC
#define GPIO_EXTI0_idle_EXTI_IRQn EXTI0_IRQn
#define SPI2_MISO_accelerometer_Pin GPIO_PIN_2
#define SPI2_MISO_accelerometer_GPIO_Port GPIOC
#define SPI2_MOSI_accelerometer_Pin GPIO_PIN_3
#define SPI2_MOSI_accelerometer_GPIO_Port GPIOC
#define TIM2_CH1_rpm_Pin GPIO_PIN_0
#define TIM2_CH1_rpm_GPIO_Port GPIOA
#define CS_oled_Pin GPIO_PIN_4
#define CS_oled_GPIO_Port GPIOA
#define SPI1_SCK_oled_Pin GPIO_PIN_5
#define SPI1_SCK_oled_GPIO_Port GPIOA
#define SPI1_MOSI_oled_Pin GPIO_PIN_7
#define SPI1_MOSI_oled_GPIO_Port GPIOA
#define DC_oled_Pin GPIO_PIN_4
#define DC_oled_GPIO_Port GPIOC
#define RESET_oled_Pin GPIO_PIN_5
#define RESET_oled_GPIO_Port GPIOC
#define ADC1_IN8_photoresistor_Pin GPIO_PIN_0
#define ADC1_IN8_photoresistor_GPIO_Port GPIOB
#define ADC1_IN9_thermistor_Pin GPIO_PIN_1
#define ADC1_IN9_thermistor_GPIO_Port GPIOB
#define SPI2_SCK_accelerometer_Pin GPIO_PIN_10
#define SPI2_SCK_accelerometer_GPIO_Port GPIOB
#define GPIO_Input_BUTTONblinkers_Pin GPIO_PIN_12
#define GPIO_Input_BUTTONblinkers_GPIO_Port GPIOB
#define GPIO_Input_BUTTONok_Pin GPIO_PIN_13
#define GPIO_Input_BUTTONok_GPIO_Port GPIOB
#define GPIO_Input_BUTTONmenu_Pin GPIO_PIN_14
#define GPIO_Input_BUTTONmenu_GPIO_Port GPIOB
#define GPIO_Output_relaydrvr1_Pin GPIO_PIN_9
#define GPIO_Output_relaydrvr1_GPIO_Port GPIOC
#define GPIO_Output_relaydrvr2_Pin GPIO_PIN_8
#define GPIO_Output_relaydrvr2_GPIO_Port GPIOA
#define GPIO_Output_relaydrvr3_Pin GPIO_PIN_9
#define GPIO_Output_relaydrvr3_GPIO_Port GPIOA
#define GPIO_Output_relaydrvr4_Pin GPIO_PIN_10
#define GPIO_Output_relaydrvr4_GPIO_Port GPIOA
#define GPIO_Output_relaydrvr5_Pin GPIO_PIN_11
#define GPIO_Output_relaydrvr5_GPIO_Port GPIOA
#define GPIO_Output_relaydrvr6_Pin GPIO_PIN_12
#define GPIO_Output_relaydrvr6_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

// pcrs_control
void control_ADC1_enable();		// turn on ADC1 module
//void control_FMPI2C1_enable();	// turn on I2C1 FM+ module
void control_I2C1_enable();		// turn on I2C1 module
void control_I2C2_enable();		// turn on I2C2 module
void control_I2C3_enable();		// turn on I2C3 module
void control_SPI1_enable();		// turn on SPI1 module
void control_SPI2_enable();		// turn on SPI2 module
//void control_SPI3_enable();		// turn on SPI3 module
void control_TIM2_enable();		// turn on TIM2 module
void control_TIM2_it_enable();	// turn on TIM2 input capture interrupt
void control_ADC1_disable();		// turn off ADC1 module
//void control_FMPI2C1_disable();		// turn off I2C1 FM+ module
void control_I2C1_disable();		// turn off I2C1 module
void control_I2C2_disable();		// turn off I2C2 module
void control_I2C3_disable();		// turn off I2C3 module
void control_SPI1_disable();		// turn off SPI1 module
void control_SPI2_disable();		// turn off SPI2 module
//void control_SPI3_disable();		// turn off SPI3 module
void control_TIM2_disable();		// turn off TIM2 module
void control_TIM2_it_disable();		// turn off TIM2 input capture interrupt

// pcrs_tasks
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
float task_RPM_Get();

// pcrs_delay
void myDelay_ms_set(myDelay_t *target, uint32_t delay_time);
bool myDelay_ms_run(myDelay_t *target);

// pcrs_menu
void menu_update();

// pcrs_icons
void menu_lights_lightbulb();
void menu_home_lightbulb_A();
void menu_home_lightbulb();
void menu_stand_sidestand();
void menu_home_sidestand();
void menu_stand_sidestand_cross();
void menu_home_sidestand_cross();
void menu_home_hazardlights_triangle();

// pcrs_clock
void clock_init();
void clock_get();
void clock_set();
bool is_leap_year(uint8_t year);
void menu_setting_clock();
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
