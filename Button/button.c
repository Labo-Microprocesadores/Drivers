/***************************************************************************//**
  @file     Button.c
  @brief    Button configurations
  @author   Grupo 2
 ******************************************************************************/


#include "button.h"
#include "SysTick.h"
#include "gpio.h"


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static Button_t buttons[BUTTON_NUM];

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void systick_callback(void)
{

	int i;

	for( i=0 ; i<BUTTON_NUM ; i++ )
	{
		bool pinState = !gpioRead(buttons[i].pin);
		if( buttons[i].lastState && !pinState )
		{
			buttons[i].wasReleased = true;
			buttons[i].wasPressed = false;
			buttons[i].currentCount = 0;
			buttons[i].lastState = false;
		}
		else if( pinState )
		{
			if( buttons[i].typefunction == LKP && (++buttons[i].currentCount) == buttons[i].lkpTime )
			{
				buttons[i].wasLkp = true;
			}
			else if( buttons[i].typefunction == TYPEMATIC && ++buttons[i].currentCount == buttons[i].typeTime)
			{
				 buttons[i].wasPressed= true;
				 buttons[i].currentCount = 0;
			}
			else if(!buttons[i].lastState)
			{
				buttons[i].wasReleased = false;
				buttons[i].wasPressed = true;
				buttons[i].lastState = true;
			}
		}
	}
}


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void buttonsInit(void)
{
	//add buttons to .h
	SysTick_AddCallback(&systick_callback, 50*SYSTICK_ISR_PERIOD_S);
}


bool wasPressed(pin_t button)
{
	int count;
	for(count=0;count<BUTTON_NUM;count++)
	{
		if(buttons[count].pin == button )
		{
			bool aux =buttons[count].wasPressed;
			if(aux)
				buttons[count].wasPressed = false;
			return aux ;
		}
	}

}

bool wasReleased(pin_t button)
{
	int count;
	for(count=0;count<BUTTON_NUM;count++)
	{
		if(buttons[count].pin == button )
		{
			bool aux =buttons[count].wasReleased;
			if(aux)
				buttons[count].wasReleased = false;

			return aux ;
		}
	}

}

bool wasLkp(pin_t button)
{
	int count;
	for(count=0;count<BUTTON_NUM;count++)
	{
		if(buttons[count].pin == button )
		{
			bool aux =buttons[count].wasLkp;
			if(aux)
				buttons[count].wasLkp = false;

			return aux ;
		}
	}
}


bool buttonConfiguration(pin_t button, int type, int time)
{
	int count;
	//I move through the button arrangement and look for the same pin to reconfigure
	for(count=0;count<BUTTON_NUM;count++)
	{
		if(buttons[count].pin == button)
		{
			buttons[count].typefunction=type;
			if(type == LKP)
				buttons[count].lkpTime = time;
			else
				buttons[count].typeTime = time;
			return true;
		}
	}
	//if the pin was not there use an empty space
	for(count=0;count<BUTTON_NUM;count++)
	{
		if(buttons[count].pin==0)
		{
			buttons[count].pin=button;
			buttons[count].typefunction=type;
			if(type == LKP)
				buttons[count].lkpTime = time;
			else
				buttons[count].typeTime = time;
			return true;
		}
	}
	//if there is no empty space for the value
	return false;
}

