/***************************************************************************//**
  @file     encoder.c
  @brief    Encoder Driver
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/

/*************************************************
 * 			INCLUDES
 *************************************************/

#include "encoder.h"
#include "SysTick.h"
#include "gpio.h"
#include "board.h"


/*************************************************
 *  	LOCAL FUNCTION DECLARATION
 ************************************************/
void lookForMoves(void);


/************************************************
 *  	VARIABLES WITH LOCAL SCOPE
 ************************************************/

static bool existingEvent = false;
static bool statusA = false;
static bool statusB = false;
static int moves = 0;


/************************************************
 * 		FUNCTION DEFINITION WITH GLOBAL SCOPE
 ************************************************/

void Encoder_Init(void)
{
	static bool init=false;
	if(init==false)
	{
		init = true;
		gpioMode(ENCODER_A,INPUT);
		gpioMode(ENCODER_B,INPUT);
		SysTick_AddCallback(&lookForMoves, 5);
		statusA = gpioRead(ENCODER_A);
		statusB = gpioRead(ENCODER_B);
	}
}

int8_t Encoder_GetMove(void)
{
	if(existingEvent)
	{
		int ret = moves;
		moves = 0;
		existingEvent = false;
		return ret;
	}
	return 0;
}

/**************************************************
 * 			LOCAL FUNCTIONS
 **************************************************/

void lookForMoves(void)
{
	if(!existingEvent)
	{
		bool auxA = gpioRead(ENCODER_A);
		bool auxB = gpioRead(ENCODER_B);
		if (statusA == auxA && statusB == auxB)
		{
			return;
		}
		else if(statusA != auxA)
		{
			if(moves != 0)
			{
				existingEvent = auxA && auxB;
			}
			else
			{
				moves--;
			}
		}
		else
		{
			if(moves != 0)
			{
				existingEvent = auxA && auxB;
			}
			else
			{
				moves++;
			}
		}
		statusA = auxA;
		statusB = auxB;
	}
}
