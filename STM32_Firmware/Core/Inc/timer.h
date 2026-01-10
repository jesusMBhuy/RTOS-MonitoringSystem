/*
 * timer.h
 *
 *  Created on: May 13, 2024
 *      Author:Jesús Martínez
 */

#ifndef TIMER_H_
#define TIMER_H_


void USER_TIM2_Delay( uint32_t time  );
void USER_TIM2_Init( void );
void USER_TIM4_Init( void );

// Función para calcular el valor del registro PSC (Prescaler)

uint16_t calculate_PSC(uint32_t time, double t_osc, uint32_t arr);

// Función para calcular el valor del registro CNT (Counter)

uint16_t calculate_CNT(uint32_t time, double t_osc, uint32_t arr, uint32_t psc);


#endif /* TIMER_H_ */
