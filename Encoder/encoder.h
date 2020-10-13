
/***************************************************************************//**
  @file     encoder.h
  @brief    encoder Header
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/

#ifndef ENCODER_H_
#define ENCODER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*
 * @brief Inicializa los pines del driver del encoder
 */
void Encoder_Init(void);

/*
 * @brief Para verificar si se realizo algun movimiento
 * @return 0-> sin movimiento, 1-> movimiento en sentido horario, -1 -> sentido antihorario
 */
int8_t Encoder_GetMove();


#endif /* ENCODER_H_ */
