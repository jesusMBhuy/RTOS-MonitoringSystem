/*
 * teclado.h
 *
 *  Created on: May 13, 2024
 *      Author: Jesus Martinez
 */

#ifndef TECLADO_H_
#define TECLADO_H_

/* Function Prototypes */
#define NUMBER_OF_ELEMENTS 4
extern unsigned short int R_POSITIONS[NUMBER_OF_ELEMENTS];
extern unsigned short int C_POSITIONS[NUMBER_OF_ELEMENTS];
extern char keys[NUMBER_OF_ELEMENTS][NUMBER_OF_ELEMENTS];

void TecladoMat_Init( void );
char Lectura_Teclado( void );
//void GetBreak(char);
//char GetDirection(char a);


// PB10 (Matrix Keyboard Row 1)


#define GPIO_ODR_10			0x1UL << 10U

// PB6 (Matrix Keyboard Row 2)


#define GPIO_ODR6			0x1UL << 6U

// PB5 (Matrix Keyboard Row 3)

#define GPIO_ODR5			0x1UL << 5U

// PB7 (Matrix Keyboard Row 4)


#define GPIO_ODR7			0x1UL << 7U

// PB1 (Matrix Keyboard Col 1)

#define GPIO_ODR1			0x1UL << 1U

// PB15 (Matrix Keyboard Col 2)


#define GPIO_ODR15			0x1UL << 15U

// PB14 (Matrix Keyboard Col 3)


#define GPIO_ODR14			0x1UL << 14U

// PB13 (Matrix Keyboard Col 4)

#define GPIO_ODR13			0x1UL << 13U


#endif /* TECLADO_H_ */
