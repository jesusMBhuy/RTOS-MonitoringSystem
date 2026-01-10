/*
 * teclado.c
 *
 *  Created on: May 13, 2024
 *      Author: Jesus Martinez
 */

#include "main.h"
#include "teclado.h"

void TecladoMat_Init(void)
{
	// Pin PB10 (Row 1) as output

	GPIOB->CRH	&=	~( GPIO_CRH_CNF10 )
		&	~( GPIO_CRH_MODE10_0 );

	GPIOB->CRH 	|=	 ( GPIO_CRH_MODE10_1 );
	GPIOB->ODR 	|=	 ( GPIO_ODR_10 );

	// Pin PB6 (Row 2) as output

	GPIOB->CRL	&=	~( GPIO_CRL_CNF6 )
		&	~( GPIO_CRL_MODE6_0 );

	GPIOB->CRL 	|=	 ( GPIO_CRL_MODE6_1 );
	GPIOB->ODR 	|=	 ( GPIO_ODR6 );

	// Pin PB5 (Row 3) as output

	GPIOB->CRL	&=	~( GPIO_CRL_CNF5 )
		&	~( GPIO_CRL_MODE5_0 );

	GPIOB->CRL 	|=	 ( GPIO_CRL_MODE5_1 );
	GPIOB->ODR 	|=	 ( GPIO_ODR5 );

	// Pin PB7 (Row 4) as output

	GPIOB->CRL	&=	~( GPIO_CRL_CNF7 )
		&	~( GPIO_CRL_MODE7_0 );

	GPIOB->CRL 	|=	 ( GPIO_CRL_MODE7_1 );
	GPIOB->ODR 	|=	 ( GPIO_ODR7 );

	// Pin PB1 (Col 1) as input pull up

	GPIOB->CRL	&=	~( GPIO_CRL_MODE1 )
		&	~( GPIO_CRL_CNF1_0 );

	GPIOB->CRL	|= 	 ( GPIO_CRL_CNF1_1 );
	GPIOB->ODR	|=	 ( GPIO_ODR1 );

	// Pin PB15 (Col 2) as input pull up

	GPIOB->CRH	&=	~( GPIO_CRH_MODE15 )
		&	~( GPIO_CRH_CNF15_0 );

	GPIOB->CRH	|= 	 ( GPIO_CRH_CNF15_1 );
	GPIOB->ODR	|=	 ( GPIO_ODR15 );

	// Pin PB14 (Col 3) as input pull up

	GPIOB->CRH	&=	~( GPIO_CRH_MODE14 )
		&	~( GPIO_CRH_CNF14_0 );

	GPIOB->CRH	|= 	 ( GPIO_CRH_CNF14_1 );
	GPIOB->ODR	|=	 ( GPIO_ODR14 );

	// Pin PB13 (Col 4) as input pull up

	GPIOB->CRH	&=	~( GPIO_CRH_MODE13 )
		&	~( GPIO_CRH_CNF13_0 );

	GPIOB->CRH	|= 	 ( GPIO_CRH_CNF13_1 );
	GPIOB->ODR	|=	 ( GPIO_ODR13 );
}




// PB10, PB4, PB5, PB3

unsigned short int R_POSITIONS[NUMBER_OF_ELEMENTS] = {10, 6, 5, 7};

// PB1, PB15, PB14, PB13

unsigned short int C_POSITIONS[NUMBER_OF_ELEMENTS] = {1, 15, 14, 13};

char keys[NUMBER_OF_ELEMENTS][NUMBER_OF_ELEMENTS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

char Lectura_Teclado( void ) {

  char selectedKey;

  for (unsigned short int i = 0; i < NUMBER_OF_ELEMENTS; i++) {

      GPIOB->BSRR = (1 << (R_POSITIONS[i] + 16));

      for (unsigned short int j = 0; j < NUMBER_OF_ELEMENTS; j++) {

	  if ((GPIOB->IDR & (1 << C_POSITIONS[j])) == 0) {
	      selectedKey = keys[i][j];
	  }
      }

      GPIOB->BSRR = (1 << R_POSITIONS[i]);
  }

  return selectedKey;
}
/*
void GetBreak(char a){
	char letra;
	float brake=0.0;

	letra=a;

	if (button == '4') {
		direction='L'
		brake = 0.0;
	}
	else if (button=='6') {
		brake = 0.0;
		direction='R'
	}
	else if (button=='2') {
		brake = 200.0;
		direction='S'
	}
	else {
		brake = 0.0;
	}
	EngTrModel_U.BrakeTorque = brake;

}


char GetDirection(char a){
	char letra;
	char direction;
	letra=a;

	if (letra == '4') {
		direction='L'; //Left
	}
	else if (letra=='2') {
		direction='R'; //Right
	}
	else if (letra=='6') {
		direction='B'; //Break
	}
	else {
		direction='S'; //Straight
	}
	return direction;
}
*/

