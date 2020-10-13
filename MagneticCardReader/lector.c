/***************************************************************************//**
  @file     lector.c
  @brief    Magnetic card reader driver
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/

/*************************************************
 * 			INCLUDES
 *************************************************/
#include "lector.h"
#include "gpio.h"
#include "board.h"


/************************************************
 * 		CONSTANTS DEFINE WITH LOCAL SCOPE
 ************************************************/
#define WORD_LONG		40
#define CHAR_LONG		5
#define BUFFER_LEN 		(2*WORD_LONG*CHAR_LONG)
#define MAX_DATA_LEN 	37


/*************************************************
 *  	LOCAL FUNCTION DECLARATION
 ************************************************/
void EnableIRQcallback(void);
void ClockIRQcallback(void);

bool find_ss_fs_es(uint16_t * ss, uint16_t * fs, uint16_t * es);
uint16_t find_fs(uint16_t ss);
uint16_t find_es(uint16_t ss);


/************************************************
 *  	VARIABLES WITH LOCAL SCOPE
 ************************************************/
static bool enable = false;
static bool event = false;
static uint16_t bit = 0;
static uint8_t message[BUFFER_LEN];


/************************************************
 * 		FUNCTION DEFINITION WITH GLOBAL SCOPE
 ************************************************/
void Lector_Init(void)
{
	gpioMode(LECTOR_ENABLE, INPUT);
	gpioMode(LECTOR_DATA, INPUT);
	gpioMode(LECTOR_CLOCK, INPUT);

	gpioIRQ(LECTOR_ENABLE, GPIO_IRQ_MODE_BOTH_EDGES, &EnableIRQcallback);
	gpioIRQ(LECTOR_CLOCK, GPIO_IRQ_MODE_FALLING_EDGE, &ClockIRQcallback); // Busco el dato cuando esta estable no al cambiar

}


bool Lector_Event(void)
{
	return event;
}

bool Lector_GetData(card_t * data)
{
	uint16_t i, j, ss, fs , es;
	card_t my_card;

	bool status = find_ss_fs_es(&ss, &fs, &es);
	if (status)
	{
		my_card.number_len = (fs - ss)/CHAR_LONG - 1;
		my_card.data_len = (es - fs)/CHAR_LONG - 1;
		for(i = 1; i <= my_card.number_len; i++)
		{
			uint8_t parity = 0;
			my_card.card_number[i-1] = 0;
			for(j=0;j<(CHAR_LONG-1);j++)
			{
				my_card.card_number[i-1] += (message[i*CHAR_LONG + j + ss]<<j);
				parity += message[i*CHAR_LONG + j + ss];
			}
			status &= ((parity%2) != message[i*CHAR_LONG + j + ss]);
		}

		for(i = 1; i <= my_card.data_len; i++)
		{
			uint8_t parity = 0;
			my_card.extra_data[i-1] = 0;
			for(j=0;j<(CHAR_LONG-1);j++)
			{
				my_card.extra_data[i-1] += (message[i*CHAR_LONG + j + fs]<<j);
				parity += message[i*CHAR_LONG + j + fs];
			}
			status &= ((parity%2) != message[i*CHAR_LONG + j + fs]);
		}
	}

	if(status)
	{
		data->data_len = my_card.data_len;
		data->number_len = my_card.number_len;
		for(i = 0; i<29; i++)
			data->extra_data[i] = my_card.extra_data[i];
		for(i = 0; i<19; i++)
			data->card_number[i] = my_card.card_number[i];
	}


	// Reset internal stuff
	event = false;
	bit = 0;
	for(i = 0; i<BUFFER_LEN; i++)
		message[i] = 0;

	return status;
}

/**************************************************
 * 			LOCAL FUNCTIONS
 **************************************************/
void EnableIRQcallback(void)
{
	if(!event)
	{
		//int new_enable = gpioRead(LECTOR_ENABLE);
		if (!enable) // falling edge
		{
			enable = true;
		}
		else
		{
			enable = false;
			event = true;
		}
	}
}

void ClockIRQcallback(void)
{
	if (enable)
	{
		uint8_t new_bit = !gpioRead(LECTOR_DATA);
		message[bit] = new_bit;
		bit++;
	}
}


bool find_ss_fs_es(uint16_t * ss, uint16_t * fs, uint16_t * es)
{
	uint16_t i, j, data, l_fs = 0, l_es = 0;
	bool find = false;
	for(i = 0; (i<(BUFFER_LEN-CHAR_LONG)) && !find; i++)
	{
		data = 0;
		for (j = 0; j<(CHAR_LONG); j++)
			data += message[i+j]<<j;

		if(data == 0xB)
		{
			l_fs = find_fs(i);
			if (l_fs != 0)
			{
				l_es = find_es(l_fs);
				if(l_es != 0)
				{
					*ss = i;
					*fs = l_fs;
					*es = l_es;
					find = true;
				}
			}
		}
	}

	return find;
}

uint16_t find_fs(uint16_t ss)
{
	uint8_t i, j, data;
	for (i = 0; i<=19 ; i++)
	{
		data = 0;
		for (j = 0; j<(CHAR_LONG); j++)
			data += message[(i+1)*CHAR_LONG+j+ss]<<j;

		if(data == 0xD)
		{
			return i*CHAR_LONG + ss;
		}
	}
	return 0;
}


uint16_t find_es(uint16_t fs)
{
	uint8_t i, j, data;
	for (i = 0; i<=29 ; i++)
	{
		data = 0;
		for (j = 0; j<(CHAR_LONG); j++)
			data += message[(i+1)*CHAR_LONG+j+fs]<<j;

		if(data == 0x1F)
		{
			return i*CHAR_LONG+fs;
		}
	}
	return 0;
}
