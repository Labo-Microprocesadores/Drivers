/***************************************************************************//**
  @file     lector.h
  @brief    Magnetic card reader Header
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/

#ifndef LECTOR_H_
#define LECTOR_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define MAX_CARD_NUMBER_LEN	19
#define MAX_EXTRA_DATA_LEN	29

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct
{
	uint8_t card_number[MAX_CARD_NUMBER_LEN];
	uint8_t extra_data[MAX_EXTRA_DATA_LEN];
	uint8_t number_len;
	uint8_t data_len;
}card_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*
 * @brief Inicializa el driver del lector de tarjetas
 */
void Lector_Init(void);

/*
 * @brief Funcion para verificar la exitencia de un evento generado por el driver
 * @return Existe el evento
 */
bool Lector_Event(void);


/*
 * @brief Recupera informacion generada en un evento
 * @param data, puntero a una estructura card_t para escribir los datos genrados
 * @return Devuelve true en caso exitoso, false si hubo un error al leer los datos
 */
bool Lector_GetData(card_t * data);

#endif /* LECTOR_H_ */
