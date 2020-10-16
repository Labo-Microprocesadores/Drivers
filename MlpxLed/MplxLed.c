/***************************************************************************//**
  @file     MplxLed.c
  @brief    Multiplexed Led Driver
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "SysTick.h"
#include "gpio.h"
#include "MplxLed.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MPLXLED_ISR_PERIOD	5	//5ms
#define DEFAULT_LED_STATE	OFF

#define LED_ARR_SIZE 3


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {NO_BLINK, BLINK, INF_BLINK, ON_FOR_TIME} LedStatus_t;

typedef struct
{
	bool state;
	LedStatus_t status;
	uint16_t counter;
	uint16_t timeOn;
	uint16_t timeOff;
	uint16_t blinkTimes;
} LedElement_t;

/*************************************************
 *  	LOCAL FUNCTION DECLARATION
 ************************************************/
void MplxLed_PISR(void);

/************************************************
 *  	VARIABLES WITH LOCAL SCOPE
 ************************************************/

LedElement_t leds[LED_ARR_SIZE];
/************************************************
 * 		FUNCTION DEFINITION WITH GLOBAL SCOPE
 ************************************************/

bool MplxLed_Init (void)
{
	static bool isInit = false;
	if(!isInit)
	{
		LedElement_t demo = {false, NO_BLINK, 0, 0, 0, 0};
		uint8_t i;
		for (i = 0; i<LED_ARR_SIZE; i++)
		{
			leds[i] = demo;
		}

		SysTick_Init();

		gpioMode(LED_LINE_I , OUTPUT);
		gpioMode(LED_LINE_II, OUTPUT);

		gpioWrite(LED_LINE_I , LOW);
		gpioWrite(LED_LINE_II, LOW);


		int systickCallbackID = SysTick_AddCallback(&MplxLed_PISR, MPLXLED_ISR_PERIOD); //5 ms
		if (systickCallbackID < 0 ) // Error
		{
			return false;
		}
	}
	return true;
}

void MplxLed_On(MplxLedID ledID)
{
	uint8_t i = (uint8_t)ledID;
	leds[i].state = true;
	leds[i].status = NO_BLINK;
	leds[i].counter = 0;
}

void MplxLed_Off(MplxLedID ledID)
{
	uint8_t i = (uint8_t)ledID;
	leds[i].state = false;
	leds[i].status = NO_BLINK;
	leds[i].counter = 0;
}

void MplxLed_Toggle(MplxLedID ledID)
{
	uint8_t i = (uint8_t)ledID;
	leds[i].state = !leds[i].state;
	leds[i].status = NO_BLINK;
	leds[i].counter = 0;
}

void MplxLed_OnForDefinedTime(MplxLedID ledID, uint16_t onTime)
{
	uint16_t time = onTime/(4*MPLXLED_ISR_PERIOD);
	uint8_t i = (uint8_t)ledID;
	leds[i].state = true;
	leds[i].status = ON_FOR_TIME;
	leds[i].counter = time;
}

void MplxLed_CustomBlink(MplxLedID ledID, uint16_t blinkTimes, uint16_t blinkPeriod, uint16_t onTime)
{
	uint16_t time = onTime/(4*MPLXLED_ISR_PERIOD);
	uint16_t period = blinkPeriod/(4*MPLXLED_ISR_PERIOD);

	if(period < time)
		return;

	uint8_t i = (uint8_t)ledID;
	leds[i].state = true;
	leds[i].status = BLINK;
	leds[i].counter = time;
	leds[i].timeOn = time;
	leds[i].timeOff = period;
	leds[i].blinkTimes = 2*blinkTimes;
}

void MplxLed_InfiniteBlink(MplxLedID ledID, MplxLedBlinkSpeed speed)
{
	uint16_t time = speed;

	uint8_t i = (uint8_t)ledID;
	leds[i].state = true;
	leds[i].status = INF_BLINK;
	leds[i].counter = time;
	leds[i].timeOn = time;
}

void MplxLed_StopInfiniteBlink(MplxLedID ledId)
{
	uint8_t i = (uint8_t)ledId;
	leds[i].state = false;
	leds[i].status = NO_BLINK;
}

void MplxLed_StopAllProcesses(MplxLedID ledId)
{
	uint8_t i = (uint8_t)ledId;
	leds[i].state = false;
	leds[i].status = NO_BLINK;
}

void MplxLed_StopAllProcessedFromAllLeds(void)
{
	LedElement_t demo = {false, NO_BLINK, 0, 0, 0, 0};
	uint8_t i;
	for (i = 0; i<LED_ARR_SIZE; i++)
	{
		leds[i] = demo;
	}
}

/**************************************************
 * 			LOCAL FUNCTIONS DEFINITIONS
 **************************************************/
void MplxLed_PISR(void)
{
	static uint8_t displayCounter = 0;

	gpioWrite(LED_LINE_I , (leds[displayCounter].state && ((displayCounter+1) & 0x01)));
	gpioWrite(LED_LINE_II, (leds[displayCounter].state && ((displayCounter+1) & 0x02)));

	LedStatus_t ledStatus = leds[displayCounter].status;
	if(ledStatus != NO_BLINK)
	{
		if(--leds[displayCounter].counter == 0)
		{
			switch(ledStatus)
			{
				case BLINK:
				{
					if(--leds[displayCounter].blinkTimes == 0)
					{
						leds[displayCounter].state = false;
						leds[displayCounter].status = NO_BLINK;
					}
					else
					{
						if(leds[displayCounter].state)
						{
							leds[displayCounter].counter = leds[displayCounter].timeOff;
						}
						else
						{
							leds[displayCounter].counter = leds[displayCounter].timeOn;
						}
						leds[displayCounter].state = !leds[displayCounter].state;
					}
					break;
				}
				case INF_BLINK:
				{
					if(leds[displayCounter].state)
					{
						leds[displayCounter].counter = leds[displayCounter].timeOn;
					}
					else
					{
						leds[displayCounter].counter = leds[displayCounter].timeOn;
					}
					leds[displayCounter].state = !leds[displayCounter].state;
					break;
				}
				case ON_FOR_TIME:
				{
					leds[displayCounter].state = false;
					leds[displayCounter].status = NO_BLINK;
					break;
				}
				case NO_BLINK: default: break;
			}
		}
	}
	displayCounter++;
	if(displayCounter == LED_ARR_SIZE)
	{
		displayCounter = 0;
	}
}
