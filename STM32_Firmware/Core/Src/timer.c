/*
 * timer.c
 *
 *  Created on: May 24, 2024
 *      Author: Jesús Martínez
 */

#include <math.h>
#include "main.h"
#include "timer.h"

#define TOSC			(31.25)



void USER_TIM2_Init( void ) {

	TIM2->SMCR	&=	~( 0x7UL << 0U );	//	Step 1 - Enable internal clock source
	TIM2->CR1	&=	~(TIM_CR1_UDIS);	//	Step 2 - UEV enabled
	TIM2->CR1	&=	~(TIM_CR1_DIR);		// 		Counter as upcounter
	TIM2->CR1	&=	~(TIM_CR1_CMS);		//		Edge-aligned mode
	TIM2->SR	&=	~(TIM_SR_UIF);		//	Step 3 - Clear Update Interrupt Flag
}

void USER_TIM4_Init(void){
	TIM4->SMCR &=  ~( 0x7UL <<  0U );//   select internal clock
	TIM4->CR1  &=  ~( 0x3UL <<  5U )//    edge-aligned mode
			   &   ~( 0x1UL <<  4U )//    upcounter
			   &   ~( 0x1UL <<  1U );//   update event (UEV) enabled
	TIM4->PSC   =    0xFFFF;//            maximum prescaler
	TIM4->EGR  |=   ( 0x1UL <<  0U );//   update the prescaler
	TIM4->CNT   =    0;//                 clear count
	TIM4->SR   &=  ~( 0x1UL <<  0U );//   clear TIM overflow-event flag
	TIM4->CR1  |=   ( 0x1UL <<  0U );//   timer enabled
}


//Función para generar el intervalo de tiempo
void USER_TIM2_Delay( uint32_t time )
{

	// Clear the Timer Update Interrupt Flag
	TIM2->SR &= ~(TIM_SR_UIF);

	uint32_t arr = 65535;
	uint16_t psc = calculate_PSC(time, TOSC, arr);
	uint16_t cnt = calculate_CNT(time, TOSC, arr, psc);

	// 	Ensure TIM2 is disabled before configuring
	TIM2->CR1 &= ~(TIM_CR1_CEN);
	TIM2->ARR = 65535;

	//	Configure initial prescaler and count values
	TIM2->PSC	=	psc;
	TIM2->CNT	=	cnt;

	//	Enable TIM2 timer to start counting
	TIM2->CR1	|=	TIM_CR1_CEN;

	//	Wait for UIF
	while(!( TIM2->SR & TIM_SR_UIF ));

	//	Stop TIM2 timer
	TIM2->CR1	&=	~(TIM_CR1_CEN);

	//TIM2->SR	&=	~(TIM_SR_UIF);		//	Clear UIF
}

uint16_t calculate_PSC(uint32_t time, double t_osc, uint32_t arr) {
    double temp = time / (t_osc * (arr + 1));
    return (uint16_t)(temp > 0 ? temp - 1 : 0);
}

uint16_t calculate_CNT(uint32_t time, double t_osc, uint32_t arr, uint32_t psc) {
    return (uint16_t)((arr + 1) - (time / (t_osc * (psc + 1))));
}

