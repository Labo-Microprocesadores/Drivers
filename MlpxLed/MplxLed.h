/***************************************************************************//**
  @file     MplxLed.h
  @brief   	Multiplexed Led Header
  @author   Grupo 2 - Lab de Micros
 ******************************************************************************/
#ifndef MPLXLED_H_
#define MPLXLED_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum MplxLedBlinkSpeed {NO_SPEED = 0, SLOW = 50, NORMAL = 25, FAST = 15} MplxLedBlinkSpeed;
typedef enum MplxLedID {LED_ONE, LED_TWO, LED_TRHEE} MplxLedID;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initialization of the Led Driver.
 * @return true if no error occurred.
 */
bool MplxLed_Init (void);

/**
 * @brief Turns a LED on.
 * @param ledID. The ID corresponding to the LED.
 */
void MplxLed_On(MplxLedID ledID);

/**
 * @brief Turns a LED off.
 * @param ledID. The ID corresponding to the LED.
 */
void MplxLed_Off(MplxLedID ledID);

/**
 * @brief Toggles a LED.
 * @param ledID. The ID corresponding to the LED.
 */
void MplxLed_Toggle(MplxLedID ledID);

/**
 * @brief 	Starts a "On For Defined Time" process.
 * 			A "On For Defined Time" process consists of a LED being ON during a defined time. After this time has elapsed, the LED is turned off.
 * @param ledID. The ID corresponding to the LED.
 * @param onTime. The time in ms. during which the LED must be ON in ms.
 * 					Example: onTime = 3000 is equivalent to 3000ms (3s)
 * WARNING If the quotient between onTime and LED_ISR_PERIOD is not an integer, it will be truncated.
 * @return A LedError indicating whether an error occurred (and its type) or not.
 */
void MplxLed_OnForDefinedTime(MplxLedID ledID, uint16_t onTime);

/**
 * @brief 	Starts a "Blink" process with custom properties.
 * 			A "Blink" process consists of a LED being turned on and off repeatedly. In other words, multiple "On For Defined Time" processes taking place repeatedly.
 * 			A "On For Defined Time" process consists of a LED being ON during a defined time. After this time has elapsed, the LED is turned off.
 * 			This function is "custom" because it gives the chance of determining the properties of the process described.
 * @param ledID. The ID corresponding to the LED.
 * @param blinkTimes. How many times the LED must be turned on and off.
 * @param blinkPeriod. The duration of an ON-OFF process during the blink in ms.
 * 							Example: blinkPeriod = 3000 is equivalent to 3000ms (3s)
 * @param onTime. The time in ms. during which the LED must be ON in an ON-OFF process.
 * @see Led_OnForDefinedTime.
 * WARNING If the quotient between blinkPeriod and LED_ISR_PERIOD is not an integer, it will be truncated.
 * @return A LedError indicating whether an error occurred (and its type) or not.
 */
void MplxLed_CustomBlink(MplxLedID ledID, uint16_t blinkTimes, uint16_t blinkPeriod, uint16_t onTime);

/**
 * @brief 	Starts a "Infinite Blink" process.
 * 			A "Infinite Blink" process consists of "toggles" taking place repeatedly and infinitely or until the process is cancelled.
 * 			In this process, the ON and OFF time of each blink are the same.
 * @param ledID. The ID corresponding to the LED.
 * @param speed. The speed of the blink.
 * @return A LedError indicating whether an error occurred (and its type) or not.
 */
void MplxLed_InfiniteBlink(MplxLedID ledID, MplxLedBlinkSpeed speed);

/**
 * @brief 	Stops a "Infinite Blink" process.
 * @param ledID. The ID corresponding to the LED.
 * @return A LedError indicating whether an error occurred (and its type) or not.
 */
void MplxLed_StopInfiniteBlink(MplxLedID ledId);

/**
 * @brief Stops all the processes ("On For Defined Time", "Blink", "Repetition Blink") and turns the LED to its default state.
 * @param ledID. The ID corresponding to the LED.
 * @return A LedError indicating whether an error occurred (and its type) or not.
 */
void MplxLed_StopAllProcesses(MplxLedID ledId);

/**
 * @brief Stops all the processes ("On For Defined Time", "Blink", "Repetition Blink") from all LEDS and turns them to their default state.
 */
void MplxLed_StopAllProcessedFromAllLeds(void);

#endif /* MPLXLED_H_ */
