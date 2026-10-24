/*
 * loadControl.c
 *
 *  Created on: Dec 9, 2023
 *      Author: Wercely
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
/* Private includes ----------------------------------------------------------*/

char receivedChar;
uint8_t uartIsBusy = 0;
char receivedBuffer[20];
uint8_t countReceived = 0;
const uint8_t toggle_load[] = { "\n\rLOAD WAS TOGGLED\r\n" };
const uint8_t turn_on_load[] = { "\n\rLOAD WAS TURNED ON\r\n" };
const uint8_t turn_off_load[] = { "\n\rLOAD WAS TURNED OFF\r\n" };
const uint8_t error_text[] = { "\n\rEXPRESSION WAS NOT RECOGNIZED\r\n" };


/**
 * @brief  Initialize variables and functions needed to firmware contolate the load as expected.
 * @param  none.
 * @retval None
 */
void loadControlInitialize(void){
	uartIsBusy = 0;
	receivedChar = 0;
	countReceived = 0;
	memset(receivedBuffer,0x00, sizeof(receivedBuffer));
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&receivedChar, 1);
}

/**
 * @brief  Set a state to the load.
 * @param  value to change the state of the load the load.
 * @retval None
 */
void loadSetValue(load_value_en value) {
	switch (value) {
	case LOAD_ON:
		if (!uartIsBusy) {
			uartIsBusy = 1;
			HAL_UART_Transmit_IT(&huart2, turn_on_load, sizeof(turn_on_load));
		}
		HAL_GPIO_WritePin(GPIO_LOAD_GPIO_Port, GPIO_LOAD_Pin,GPIO_PIN_SET);
		break;
	case LOAD_OFF:
		if (!uartIsBusy) {
			uartIsBusy = 1;
			HAL_UART_Transmit_IT(&huart2, turn_off_load, sizeof(turn_off_load));
		}
		HAL_GPIO_WritePin(GPIO_LOAD_GPIO_Port, GPIO_LOAD_Pin,GPIO_PIN_RESET);
		break;
	case LOAD_TOGGLE:
		if (!uartIsBusy) {
			uartIsBusy = 1;
			HAL_UART_Transmit_IT(&huart2, toggle_load, sizeof(toggle_load));
		}
		HAL_GPIO_TogglePin(GPIO_LOAD_GPIO_Port, GPIO_LOAD_Pin);
		break;
	default:
		if (!uartIsBusy) {
			uartIsBusy = 1;
			HAL_UART_Transmit_IT(&huart2, error_text, sizeof(error_text));
		}
		break;
	}
}


/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == CONTROL_Pin) {
		loadSetValue(LOAD_TOGGLE);
	}
}

/**
 * @brief Tx Transfer completed callback.
 * @param huart UART handle.
 * @retval None
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	uartIsBusy = 0;
}

/**
 * @brief  Rx Transfer completed callback.
 * @param  huart UART handle.
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (countReceived >= sizeof(receivedBuffer)) {
		countReceived = 0;
		loadSetValue(LOAD_ERROR);
		memset(receivedBuffer,0x00, sizeof(receivedBuffer));
	}
	receivedBuffer[countReceived++] = receivedChar;

	for (int i = 0; i < sizeof(receivedBuffer); i++){
		if((strncasecmp((const char *)&receivedBuffer[i],"turn on",sizeof("turn on")) == 0) || (strncasecmp((const char *)&receivedBuffer[i],"liga",sizeof("liga")) == 0) ){
			countReceived = 0;
			loadSetValue(LOAD_ON);
			memset(receivedBuffer,0x00, sizeof(receivedBuffer));
		}
		if((strncasecmp((const char *)&receivedBuffer[i],"turn off",sizeof("turn off")) == 0) || (strncasecmp((const char *)&receivedBuffer[i],"desliga",sizeof("desliga")) == 0) ){
			countReceived = 0;
			loadSetValue(LOAD_OFF);
			memset(receivedBuffer,0x00, sizeof(receivedBuffer));
		}
	}

	HAL_UART_Receive_IT(&huart2, (uint8_t *)&receivedChar, 1);
}
