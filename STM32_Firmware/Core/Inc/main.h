/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
extern "C"
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

// PA0 (ADC0 Input)






/* ADC registers */


// NVIC (Nested Vectored Interrupt Controller) Registers
typedef struct {
    volatile uint32_t ISER[3U]; // ISER: Interrupt Set-Enable Registers. Tienen 3 registros de 32 bits.
    volatile uint32_t RESERVED0[29U]; // Espacio reservado entre ISER y ICER.
    volatile uint32_t ICER[3U]; // ICER: Interrupt Clear-Enable Registers. Tienen 3 registros de 32 bits.
    volatile uint32_t RESERVED1[29U]; // Espacio reservado entre ICER y ISPR.
    volatile uint32_t ISPR[3U]; // ISPR: Interrupt Set-Pending Registers. Tienen 3 registros de 32 bits.
    volatile uint32_t RESERVED2[29U]; // Espacio reservado entre ISPR y ICPR.
    volatile uint32_t ICPR[3U]; // ICPR: Interrupt Clear-Pending Registers. Tienen 3 registros de 32 bits.
    volatile uint32_t RESERVED3[29U]; // Espacio reservado entre ICPR y IABR.
    volatile uint32_t IABR[3U]; // IABR: Interrupt Active Bit Registers. Tienen 3 registros de 32 bits.
    volatile uint32_t RESERVED4[61U]; // Espacio reservado adicional.
    volatile uint8_t IPR[84U]; // IPR: Interrupt Priority Registers. Tienen 84 registros de 8 bits.
    volatile uint32_t RESERVED5[683U]; // Espacio reservado antes del STIR.
    volatile uint32_t STIR; // STIR: Software Trigger Interrupt Register. Registro de 32 bits.
} NVIC_TypeDef;

/* USART Registers */


/* Flash memory interface registers */

/* BASE ADDRESSESS */

/* Structure defines */
#define RCC         ((RCC_TypeDef *)RCC_BASE)



// PA0 (ADC0 Input)



// PA9 (USART1_TX)





void USER_RCC_Init( void );
void USER_GPIO_Init( void );
/*
void Task1_Init( void );
void Task2_Init( char );
void Task3_Init( void );
void Task4_Init(void);

*/


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
