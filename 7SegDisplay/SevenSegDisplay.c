/***************************************************************************//**
  @file     7SegDisplay.c
  @brief    Display configurations
  @author   Grupo 2
 ******************************************************************************/

#include "SevenSegDisplay.h"
#include "SysTick.h"
#include "gpio.h"
#include "Timer.h"
#include "board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SCREEN_SIZE 4
#define BLINK_TIME 200
#define PERIOD 5

/*************************************************
 *  	LOCAL FUNCTION DECLARATION
 ************************************************/

void SevenSegDisplay_PISR(void);
bool SevenSegDisplay_PrintCharacter(uint8_t character);

/************************************************
 *  	VARIABLES WITH LOCAL SCOPE
 ************************************************/
static sevenSeg_t screen[SCREEN_SIZE] = { {NONE, false, true, 0},
										  {NONE, false, true, 0},
										  {NONE, false, true, 0},
										  {NONE, false, true, 0} };

static pin_t displayPins[SEG_LEN] = {PIN_SEGA, PIN_SEGB, PIN_SEGC, PIN_SEGD,
							 	 	 PIN_SEGE, PIN_SEGF, PIN_SEGG, PIN_SEGDP};

static pin_t selectPins[SEL_LEN] = {PIN_SEL0, PIN_SEL1};

static bright_t brightness = MAX;

/************************************************
 * 		FUNCTION DEFINITION WITH GLOBAL SCOPE
 ************************************************/
bool SevenSegDisplay_Init(void)
{
	static bool isInit = false;
	if(!isInit)
	{
		SysTick_Init();
		uint8_t count;
		for(count=0; count<SEG_LEN ;count++)
		{
			gpioMode(displayPins[count], OUTPUT);
		}
		for(count=0; count<SEL_LEN ;count++)
		{
			gpioMode(selectPins[count], OUTPUT);
		}
	    //pongo los 2 pin a demultiplexar en 00 para que solo se prenda un 7segmentos
	    gpioWrite (selectPins[0], false);
	    gpioWrite (selectPins[1], false);

		int systickCallbackID = SysTick_AddCallback(&SevenSegDisplay_PISR, 1); //1 ms
		if (systickCallbackID < 0 ) // Error
		{
			return false;
		}
	//idCounter = 1;
	}
	return true;
}


void SevenSegDisplay_ChangeCharacter(uint8_t screen_char, uint8_t new_char)
{
	if (screen_char < SCREEN_SIZE)
	{
		screen[screen_char].character = new_char;
	}
}

bool SevenSegDisplay_BlinkScreen(bool state)
{
	//set all variables on state (true or false)
	uint8_t count;
	for(count=0; count<SCREEN_SIZE; count++)
	{
		screen[count].blink = state;
		screen[count].blinkCounter = BLINK_TIME;
		screen[count].blinkState = true;
	}

	return true;
}

bool SevenSegDisplay_BlinkCharacter(uint8_t digit)
{
	//controls if the digit is valid
	if( (digit>=0 && digit <=3) || (digit == RESET_BLINK) )
	{
		//if the digit is valid reset all blink variables
		uint8_t count;
		for(count=0; count<SCREEN_SIZE; count++)
		{
			screen[count].blink=false;
			screen[count].blinkCounter = BLINK_TIME;
			screen[count].blinkState = true;
		}
		//if I want to put a digit to blink
		if(digit!=RESET_BLINK)
		{
			screen[digit].blink=true;
		}
	}else
	{
		//if the digit wasn't validate return false
		return false;
	}
	return true;
}

void SevenSegDisplay_SetBright(bright_t new_bright)
{
	brightness = new_bright;
}
/**************************************************
 * 			LOCAL FUNCTIONS DEFINITIONS
 **************************************************/

/**
 * @brief print one character on one 7 segments display
 * @param character to print
 * @param array with pins to 7 segments display
 * @return printed succeed
 */
bool SevenSegDisplay_PrintCharacter(uint8_t character)
{
	int count;

	for(count=0; count<SEG_LEN; count++)
	{
		gpioWrite(displayPins[count], (character & (1<<count)) != 0);
	}
	return 0;
}

void SevenSegDisplay_PISR(void)
{
	static uint8_t displayCounter = 0;
	static int8_t  currBright = MAX;
	static uint8_t window = PERIOD;

	uint8_t dataToPrint = (screen[displayCounter].blinkState && (currBright > 0)) ? screen[displayCounter].character: NONE;

	gpioWrite(selectPins[0], (displayCounter & (1   )) != 0);
	gpioWrite(selectPins[1], (displayCounter & (1<<1)) != 0);
	SevenSegDisplay_PrintCharacter(dataToPrint);

	currBright--;

	if(--window == 0)
	{
		displayCounter++;
		window = PERIOD;
		currBright = brightness;
		if(displayCounter == SCREEN_SIZE)
		{
			displayCounter = 0;
		}

	}

	if(screen[displayCounter].blink)
	{
		if(--screen[displayCounter].blinkCounter == 0)
		{
			screen[displayCounter].blinkCounter = BLINK_TIME;
			screen[displayCounter].blinkState = !screen[displayCounter].blinkState;
		}
	}

}

void SevenSegDisplay_EraseScreen(void)
{
	for(int i = 1; i<((int)(sizeof(screen)/sizeof(screen[0]))); i++)
	{
		screen[i].character = NONE;
	}
}


