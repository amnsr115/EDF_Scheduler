/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )
	


	    BaseType_t xTaskPeriodicCreate( TaskFunction_t pxTaskCode,
																		const char * const pcName,
																		const configSTACK_DEPTH_TYPE usStackDepth,
																		void * const pvParameters,
																		UBaseType_t uxPriority,
																		TaskHandle_t * const pxCreatedTask,
																		TickType_t period);
	
/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

uint8_t B1_Rising_Edge = 0;
uint8_t B1_Falling_Edge = 0;
uint8_t B2_Rising_Edge = 0;
uint8_t B2_Falling_Edge = 0;

uint8_t Periodic_Flag = 0;
																		
																		
																		
																
/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */

void Button_1_Monitor(void *pvParameters);
void Button_2_Monitor(void *pvParameters);
void Periodic_Transmitter(void *pvParameters);
void Uart_Receiver(void *pvParameters);
void Load_1_Simulation(void *pvParameters);
void Load_2_Simulation(void *pvParameters);


typedef const signed char* Message_t;



void Button_1_Monitor(void *pvParameters)
{
	TickType_t xLastWakeTimeB1=0;	
	pinState_t Button1_Signal = PIN_IS_LOW;
	
	for(;;)
	{
				while(Button1_Signal == PIN_IS_LOW)
		{
			if(GPIO_read(PORT_0, PIN0)==PIN_IS_HIGH)
			{
				B1_Rising_Edge = 1;//Send Rising Edge
				Button1_Signal = PIN_IS_HIGH;
				break;
			}
			GPIO_write(PORT_0, PIN5, PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTimeB1, 50);
			GPIO_write(PORT_0, PIN5, PIN_IS_HIGH);
		}
		while(Button1_Signal == PIN_IS_HIGH)
		{
			if(GPIO_read(PORT_0, PIN0)==PIN_IS_LOW)
			{
				B1_Falling_Edge = 1; //Send Falling Edge
				Button1_Signal = PIN_IS_LOW;
				break;
			}
			GPIO_write(PORT_0, PIN5, PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTimeB1, 50);
			GPIO_write(PORT_0, PIN5, PIN_IS_HIGH);
		}
		//{Periodicity: 50, Deadline: 50}
	}
}
void Button_2_Monitor(void *pvParameters)
{
	TickType_t xLastWakeTimeB2=0;
	
	pinState_t Button2_Signal = PIN_IS_LOW;
	for(;;)
	{
		while(Button2_Signal == PIN_IS_LOW)
		{
			if(GPIO_read(PORT_0, PIN1)==PIN_IS_HIGH)
			{
				B2_Rising_Edge = 1; //Send Rising Edge
				Button2_Signal = PIN_IS_HIGH;
				break;
			}
			GPIO_write(PORT_0, PIN6, PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTimeB2, 50);
			GPIO_write(PORT_0, PIN6, PIN_IS_HIGH);
		}
		while(Button2_Signal == PIN_IS_HIGH)
		{
			if(GPIO_read(PORT_0, PIN1)==PIN_IS_LOW)
			{
				B2_Falling_Edge = 1;//Send Falling Edge
				Button2_Signal = PIN_IS_LOW;
				break;
			}
			GPIO_write(PORT_0, PIN6, PIN_IS_LOW);
			vTaskDelayUntil( &xLastWakeTimeB2, 50);
			GPIO_write(PORT_0, PIN6, PIN_IS_HIGH);
		}
		//{Periodicity: 50, Deadline: 50}
	}
}
void Periodic_Transmitter(void *pvParameters)
{
	TickType_t xLastWakeTimePeriodic=0;
	for(;;)
	{
		GPIO_write(PORT_0, PIN7, PIN_IS_LOW);
		vTaskDelayUntil( &xLastWakeTimePeriodic, 100);
		GPIO_write(PORT_0, PIN7, PIN_IS_HIGH);
		Periodic_Flag = 1;
		
		//{Periodicity: 100, Deadline: 100}
	}
}
void Uart_Receiver(void *pvParameters)
{
	TickType_t xLastWakeTimeUart=0;
		for(;;)
	{
		GPIO_write(PORT_0, PIN8, PIN_IS_LOW);
		vTaskDelayUntil( &xLastWakeTimeUart, 20);
		GPIO_write(PORT_0, PIN8, PIN_IS_HIGH);
		
		if(B1_Rising_Edge==1)
		{
			vSerialPutString((Message_t)"B1 Rising Edge", 30);
			B1_Rising_Edge = 0;
		}
		if(B1_Falling_Edge==1)
		{
			vSerialPutString((Message_t)"B1 Falling Edge", 30);
			B1_Falling_Edge = 0;
		}
		if(B2_Rising_Edge==1)
		{
			vSerialPutString((Message_t)"B2 Rising Edge", 30);
			B2_Rising_Edge = 0;
		}
		if(B2_Falling_Edge==1)
		{
			vSerialPutString((Message_t)"B2 Falling Edge", 30);
			B2_Falling_Edge = 0;
		}
		if(Periodic_Flag == 1)
		{
			vSerialPutString((Message_t)"Periodic Task", 30);
			Periodic_Flag = 0;
		}

		
		//{Periodicity: 20, Deadline: 20}
	}
}
void Load_1_Simulation(void *pvParameters)
{
	TickType_t xLastWakeTimeLoad1 = 0;
	for(;;)
	{
		GPIO_write(PORT_0, PIN9, PIN_IS_LOW);
		vTaskDelayUntil( &xLastWakeTimeLoad1, 10);
		GPIO_write(PORT_0, PIN9, PIN_IS_HIGH);
		for(unsigned int i=0; i<((33000)-1);i++){}
		//{Periodicity: 10, Deadline: 10}
		
	}
	
}
void Load_2_Simulation(void *pvParameters)
{
	TickType_t xLastWakeTimeLoad2 = 0;
		for(;;)
	{
		GPIO_write(PORT_0, PIN4, PIN_IS_LOW);
		vTaskDelayUntil( &xLastWakeTimeLoad2, 100);
		GPIO_write(PORT_0, PIN4, PIN_IS_HIGH);
		for(unsigned int i=0; i<((47000)-1);i++){}
		//{Periodicity: 100, Deadline: 100}
		
	}
}



void vApplicationIdleHook( void )
{
GPIO_write(PORT_0, PIN2, PIN_IS_HIGH);
}


void vApplicationTickHook(void){
GPIO_write(PORT_0, PIN2, PIN_IS_LOW);
GPIO_write(PORT_0, PIN3, PIN_IS_HIGH);
GPIO_write(PORT_0, PIN3, PIN_IS_LOW);
}


TaskHandle_t Button1_Handler, Button2_Handler, Periodic_Handler, Uart_Handler, Load1_Handler, Load2_Handler; 
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	//GPIO_write(PORT_0, PIN0, PIN_IS_HIGH);
	
  /* Create Tasks here */
	xTaskPeriodicCreate( Button_1_Monitor, 		( const char * ) "Button_1_Monitor",		configMINIMAL_STACK_SIZE, NULL,1, &Button1_Handler, 	50);
	xTaskPeriodicCreate( Button_2_Monitor, 		( const char * ) "Button_2_Monitor",		configMINIMAL_STACK_SIZE, NULL,1, &Button2_Handler, 	50);
	xTaskPeriodicCreate( Periodic_Transmitter,( const char * ) "Periodic_Transmitter",configMINIMAL_STACK_SIZE,	NULL,1, &Periodic_Handler,	100);
	xTaskPeriodicCreate( Uart_Receiver, 			( const char * ) "Uart_Receiver",				configMINIMAL_STACK_SIZE, NULL,1, &Uart_Handler, 			20);
	xTaskPeriodicCreate( Load_1_Simulation, 	( const char * ) "Load_1_Simulation",		configMINIMAL_STACK_SIZE, NULL,1, &Load1_Handler, 		10);
	xTaskPeriodicCreate( Load_2_Simulation, 	( const char * ) "Load_2_Simulation",		configMINIMAL_STACK_SIZE, NULL,1, &Load2_Handler, 		100);
	


	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


