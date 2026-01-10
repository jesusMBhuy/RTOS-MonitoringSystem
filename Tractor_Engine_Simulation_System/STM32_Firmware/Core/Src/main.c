#include "main.h"
#include "cmsis_os.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "EngTrModel.h"
#include "rtwtypes.h"
#include "lcd.h"
#include "uart.h"
#include "timer.h"
#include "teclado.h"

// Jesús Javier Martínez Hernández - A00833296
// Diego Hilario López Rodriguez - A00836492
// Diego José Roca Rodriguez - A00836390
// Victor Alejandro Meneses Garza - A01384002

//Private variables---------------------

osThreadId tarea1Handle;
osThreadId tarea2Handle;
osThreadId tarea3Handle;
osThreadId tarea4Handle;

osMessageQId msgQueueHandle;

osMutexId Mutex1Handle;

// Declaración de funciones
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void Task1_Init(void);
void encender_led(uint16_t);
void apagar_led(uint16_t );
void alternar_led(uint16_t );
void USER_ADC1_Enable( void );
void USER_ADC1_Calibration( void );
uint16_t ADC_ConvertirValor( void );
void startTask1(void);
void startTask2(void);
void startTask3(void);
void startTask4(void);


  // Estructura para enviar datos a través de la cola
  typedef struct {
      float acceleration;
      char recibido;
  } AccelerationMessage;


// Función principal
int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	Task1_Init();

	osMutexDef(Mutex1);
	Mutex1Handle = osMutexCreate(osMutex(Mutex1));

	// RTOS Tasks
	osThreadDef(task4, startTask4, osPriorityNormal, 0, 384);
	tarea4Handle = osThreadCreate(osThread(task4), NULL);

	osThreadDef(task2, startTask2, osPriorityNormal, 0, 384);
	tarea2Handle = osThreadCreate(osThread(task2), NULL);

	osThreadDef(task1, startTask1, osPriorityNormal, 0, 384);
	tarea1Handle = osThreadCreate(osThread(task1), NULL);


	osThreadDef(task3, startTask3, osPriorityNormal, 0, 384);
	tarea3Handle = osThreadCreate(osThread(task3), NULL);

	// Queue para la estructura AccelerationMessage
	osMessageQDef(msgQueue, 4, AccelerationMessage);
	msgQueueHandle = osMessageCreate(osMessageQ(msgQueue), NULL);
	osKernelStart();



	while (1)
	{
	}

}

void startTask1(void){

	uint16_t adcValue;
	float throttle;
	AccelerationMessage msg;
	char buttonRasp;



	for (;;) {

		adcValue = ADC_ConvertirValor();
		throttle = ((adcValue * 100) / 4095.0);
		if (throttle < 5.0) {
			throttle = 5.0;
		}
		buttonRasp = USER_USART1_Receive();

		// Asignar el valor de aceleración a la estructura y enviarlo a la cola
		msg.recibido = buttonRasp;
		msg.acceleration = throttle;
		osMessagePut(msgQueueHandle, (uint32_t)&msg, 1);
		//printf("Task1 execution time: %ld s\r\n", osKernelSysTick());
		osDelay(33); // Agregar un pequeño delay para permitir que otras tareas se ejecuten
	}
}
void startTask2(void){
	osEvent evt;
	char button;

	for (;;) {

		evt = osMessageGet(msgQueueHandle, osWaitForever);
		if (evt.status == osEventMessage) {
			AccelerationMessage* receivedMsg = (AccelerationMessage*)evt.value.p;
			float throttle = receivedMsg->acceleration;
			char buttonRasp = receivedMsg->recibido;
			EngTrModel_U.Throttle = throttle;


			button = Lectura_Teclado();
			if (button == '4' ) {
				osMutexWait(Mutex1Handle, osWaitForever);
				LCD_Clear();
				LCD_Set_Cursor(1, 0);
				LCD_Put_Str( "Dir: IZQUIERDA" );
				apagar_led(11);
				encender_led(12);
				osMutexRelease(Mutex1Handle);

			} else if (button == '6' ) {
				osMutexWait(Mutex1Handle, osWaitForever);
				LCD_Clear();
				LCD_Set_Cursor(1, 0);
				LCD_Put_Str( "Dir: DERECHA" );
				apagar_led(12);
				encender_led(11);
				osMutexRelease(Mutex1Handle);

			}
			else if (button == '2' ) {
				EngTrModel_U.BrakeTorque = 1000.0;
				encender_led(11);
				encender_led(12);
			}
			else if(buttonRasp=='F'){
				EngTrModel_U.BrakeTorque = 3000.0;
				osMutexWait(Mutex1Handle, osWaitForever);
				LCD_Clear();
				LCD_Set_Cursor(1, 0);
				LCD_Put_Str( "FRENO MANUAL" );
				encender_led(11);
				encender_led(12);
				osMutexRelease(Mutex1Handle);

			}
			else if(buttonRasp=='R'){
				apagar_led(12);
				alternar_led(11);
			}
			else if(buttonRasp=='L'){
				apagar_led(11);
				alternar_led(12);
			}

			else {
				EngTrModel_U.BrakeTorque = 0.0;
				apagar_led(12);
				apagar_led(11);

			}

			EngTrModel_step(); // Se actualizan los valores en el modelo
		}

		 //printf("Task2 execution time: %ld s\r\n", osKernelSysTick());
	}
	osDelay(200);
}
void startTask3(void){
	for(;;){
		printf("Velocidad del tractor: %f,",EngTrModel_Y.VehicleSpeed );
		printf("Velocidad del motor: %f,", EngTrModel_Y.EngineSpeed);
		printf("Acelerador: %f,",EngTrModel_U.Throttle);
		printf("Marcha: %f\n\r",EngTrModel_Y.Gear );
		//printf("%f,%f,%f\n\r",EngTrModel_Y.VehicleSpeed,EngTrModel_Y.Gear,EngTrModel_Y.EngineSpeed);


		//printf("Task3 execution time: %ld s\r\n", osKernelSysTick());
		osDelay(463);//	50ms
	}
}
void startTask4(void){
	for(;;){
		//LCD_Clear();
		//LCD_Set_Cursor(1, 0);
		//LCD_Put_Str(direction);
		osMutexWait(Mutex1Handle, osWaitForever);
		LCD_Set_Cursor(1, 0);
		LCD_Put_Str( " VT:" );
		LCD_Put_Num( EngTrModel_Y.VehicleSpeed );
		LCD_Set_Cursor(1, 8);
		LCD_Put_Str( "VM:");
		LCD_Put_Num( EngTrModel_Y.EngineSpeed );
		LCD_Set_Cursor( 2, 0 );
		LCD_Put_Str( "GE:");
		LCD_Put_Num( EngTrModel_Y.Gear );
		LCD_Set_Cursor( 2, 8 );
		LCD_Put_Str( " AC:");
		LCD_Put_Num( EngTrModel_U.Throttle );
		osMutexRelease(Mutex1Handle);

		//printf("Task4 execution time: %ld s\r\n", osKernelSysTick());
		osDelay(582); // Delay for 200ms
	}
}


// Configuración del reloj del sistema
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

// Inicialización de los puertos GPIO
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
}




void Task1_Init(void){
	USER_RCC_Init();
	USER_TIM4_Init();
	USER_TIM2_Init();
	EngTrModel_initialize();
	USER_GPIO_Init();
	USER_ADC1_Enable(); //Activamos el puerto ADC
	USER_USART1_Init(); //Iniciamos la UART
	TecladoMat_Init(); //Iniciamos el teclado Matricial
	LCD_Init(); //Iniciamos el periferico de la LCD
}



void USER_GPIO_Init( void ){
	// Pin PA0 as analog input

	GPIOA->CRL	&=	~( GPIO_CRL_MODE0 )
		&	~( GPIO_CRL_CNF0 );
	// Pin PA9 (USART1_TX) as alternate function output push-pull, max speed 10MHz

	GPIOA->CRH	&=	~( GPIO_CRH_CNF9_0 )
		&	~( GPIO_CRH_MODE9 );

	GPIOA->CRH	|=	 ( GPIO_CRH_CNF9 )
		|	 ( GPIO_CRH_MODE9_0 );

	//Pin PA10 (USART1_RX) as alternate function output push-pull, max speed 10MHz

	GPIOA->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_MODE10);
	GPIOA->CRH |= GPIO_CRH_CNF10_0;

	// Configuración del PIN PB11 como salida push-pull
	GPIOB->CRH &= ~((0xF << 12));  // Limpiar bits de configuración para el PIN 11
	GPIOB->CRH |=  (0x1 << 12);    // Configurar como salida de 10 MHz (MODE11[1:0] = 01, CNF11[1:0] = 00)

	// Configuración del PIN PB12 como salida push-pull
	GPIOB->CRH &= ~((0xF << 16));  // Limpiar bits de configuración para el PIN 12
	GPIOB->CRH |=  (0x1 << 16);    // Configurar como salida de 10 MHz (MODE12[1:0] = 01, CNF12[1:0] = 00)
}
void encender_led(uint16_t pin) {
    GPIOB->ODR |= (1 << pin);  // Establecer el pin correspondiente (alta)
}

void apagar_led(uint16_t pin) {
    GPIOB->ODR &= ~(1 << pin);  // Restablecer el pin correspondiente (baja)
}
void alternar_led(uint16_t pin) {
    GPIOB->ODR ^= (1 << pin);  // Cambiar el estado del pin correspondiente
    HAL_Delay(.15);
}

void USER_RCC_Init(void) {
	// RCC_APB2ENR modified to IO port A clock enable

	  RCC->APB2ENR |= 	RCC_APB2ENR_IOPAEN;	// To set IOPAEN bit

	  // RCC_APB2ENR modified to IO port B clock enable

	  RCC->APB2ENR |= 	RCC_APB2ENR_IOPBEN;	// To set IOPBEN bit

	  // RCC_APB2ENR modified to IO port C clock enable

	  RCC->APB2ENR	|=	RCC_APB2ENR_IOPCEN;	// To set IOPCEN bit

	  // RCC_APB1ENR modified to enable the clock for TIM2 and TIM4

	  RCC->APB1ENR |= 	RCC_APB1ENR_TIM2EN;	// To set TIM2EN bit
	  RCC->APB1ENR |=   RCC_APB1ENR_TIM4EN; //To set TIM4EN bit

	  // RCC_APB2ENR and RCC_CFGR modified to enable and adjust the clock for ADC1

	  RCC->APB2ENR	|=	RCC_APB2ENR_ADC1EN;	// Enable clock for ADC1
	  RCC->CFGR	|=	RCC_CFGR_ADCPRE;        // Adjust ADC input clock

	  // RCC_APB2ENR modified to enable the clock for USART1

	  RCC->APB2ENR |= RCC_APB2ENR_USART1EN; 	// To set USART1EN bit

	  /* System Clock (SYSCLK) configuration for 64 MHz */

	  // Two wait states latency, if SYSCLK > 48 MHz

	  FLASH->ACR	&=	~( 0x5UL << 0U );
	  FLASH->ACR	|=	( 0x2UL << 0U );

	  // PLL HSI clock /2 selected as PLL input clock

	  RCC->CFGR	&=	~( 0x1UL << 16U )
			&	~( 0x7UL << 11U )	// APB2 pre-scaler /1
			&	~( 0x3UL << 8U )        // APB1 pre-scaler /2 (APB1 must not exceed 36 MHz)
			&	~( 0xFUL << 4U );	// AHB pre-scaler /1

	  // PLL input clock x 16 (PLLMUL bits)

	  RCC->CFGR	|=	( 0xFUL << 18U )
			|	( 0X4UL << 8U );	// APB1 pre-scaler /2
	  RCC->CR	|=	( 0x1UL << 24U );	// PLL2 ON

	  // Wait until PLL is locked

	  while( !(RCC->CR & ~( 0x1UL << 25U )));

	  // PLL used as system clock (SW bits)

	  RCC->CFGR	&=	~( 0x1UL << 0U );
	  RCC->CFGR	|=	( 0x2UL << 0U );

	  // Wait until PLL is switched

	  while( 0x8UL != ( RCC->CFGR & 0xCUL ));
}

void USER_ADC1_Enable(void) {
    //Codigo nuevo
	ADC1->CR1	&=	~(ADC_CR1_DUALMOD);	// Step 1 - Select independent mode
	ADC1->CR2	|=	ADC_CR2_CONT;		// Step 2 - Select conversion mode
	ADC1->CR2	&= 	~(ADC_CR2_ALIGN);	//	and format for ADC result
	ADC1->SMPR2	&= 	~(ADC_SMPR2_SMP0);	// Step 3 - Select the sample time for the
						//	ADC channel
	ADC1->SQR1	&= 	~(ADC_SQR1_L);		// Step 4 - Select the sequence and number of
						//	conversions for the ADC regular channels
	ADC1->SQR3	&= 	~(ADC_SQR3_SQ1);	// Step 5 - Select the channel for the first ADC
						//	conversion
	ADC1->CR2	|=	ADC_CR2_ADON;		// Step 6 - Enable the ADC module
	ADC1->CR2	|=	ADC_CR2_CAL;		// Step 7 - Perform a calibration after
						// 	each power-up
	while ( ADC1->CR2 & ADC_CR2_CAL );		// Step 8 - Wait until the bit is reset by
						//	hardware after calibration is complete
}

uint16_t ADC_ConvertirValor(void) {
    // Start conversion
    ADC1->CR2 |= ADC_CR2_ADON; // Start conversion
    // Wait until conversion is done
    while (!(ADC1->SR & ADC_SR_EOC)); // Wait for the end of conversion (EOC) flag
    // Read ADC result
    uint16_t valorADC = ADC1->DR; // Read the data register to get ADC result and clear EOC flag

    return valorADC;
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
}

// Manejador de errores
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
