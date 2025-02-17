/*
 * wifi_module.c
 *
 *  Created on: Feb 3, 2025
 *      Author: miczu
 */
#include <string.h>
#include "wifi_module.h"

#define HUART_ESP8266 huart1

#define SIZE_INFO_LENGTH				3U
#define DATA_SIZE_MAX					30U
#define NEGATIVE_SIGN_IN_ASCII_OFFSET	45U
#define DIGIT_ZERO_IN_ASCII_OFFSET		48U
#define DIGIT_NINE_IN_ASCII_OFFSET		57U

#define REQUEST_RESPONSE_TIME_MAX 		2U

#define __HAL_UART_FLUSH_RDRREGISTER(__HANDLE__)  \
  do{                \
    SET_BIT((__HANDLE__)->Instance->RQR, UART_RXDATA_FLUSH_REQUEST); \
  }  while(0U)

volatile bool wifiDataReq = false;
WiFi_WeatherData_t wifiData;

volatile bool dma_transfer_cplt = false;
static bool data_size_correct = true;

uint32_t tickstart;

// volatile?
char rx_data_size[SIZE_INFO_LENGTH];
char rx_data[DATA_SIZE_MAX];
uint16_t data_size;

static int16_t myPow(int16_t base, int16_t exponent) {
	int16_t number = 1;

    for (int16_t i = 0; i < exponent; ++i)
        number *= base;

    return number;
}

static bool isNegSign(uint8_t ch) {
	if (ch == NEGATIVE_SIGN_IN_ASCII_OFFSET) {
		return true;
	} else {
		return false;
	}
}

static bool isDigit(uint8_t ch) {
	if ((ch >= DIGIT_ZERO_IN_ASCII_OFFSET) && (ch <= DIGIT_NINE_IN_ASCII_OFFSET)) {
		return true;
	} else {
		return false;
	}
}

static int16_t convertCharArrayToNumber(char* srcArray, int16_t arraySize) {
	int16_t number = 0;
	int16_t digit;
	int16_t sign = 1;

	for (int16_t i = 0; i < arraySize; i++) {
		char ch = *(srcArray + i);
		if (isNegSign(ch)) {
			sign = -1;
			continue;
		}
		else if (!isDigit(ch)) {
			return 0;
		}
		digit = (int16_t)(ch) - DIGIT_ZERO_IN_ASCII_OFFSET;
		// digits are processed from most significant to least
		number += sign * digit * myPow(10, (arraySize - 1 - i));
		sign = 1;
	}
	return number;
}

void esp8266_requestDataSize(void) {
	memset(rx_data_size, 0, sizeof(rx_data_size));
	memset(rx_data, 0, sizeof(rx_data));
	data_size = 0;

	__HAL_UART_FLUSH_RDRREGISTER(&HUART_ESP8266);
	__HAL_UART_CLEAR_OREFLAG(&HUART_ESP8266);

	HAL_UART_Receive_DMA(&HUART_ESP8266, (uint8_t*) rx_data_size, SIZE_INFO_LENGTH);
	HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_RESET);

	tickstart = HAL_GetTick();
	while (!dma_tranfer_cplt) {
		if ((HAL_GetTick() - tickstart) > REQUEST_RESPONSE_TIME_MAX) {
			break;
		};
	}
	dma_tranfer_cplt = false;

	data_size = (uint16_t)convertCharArrayToNumber(rx_data_size, SIZE_INFO_LENGTH);
	if ((data_size <= 0U) || (data_size > DATA_SIZE_MAX)) {
		data_size_correct = false;
	} else {
		data_size_correct = true;
	}
}

void esp8266_requestData(void) {
	memset(rx_data_size, 0, sizeof(rx_data_size));
	memset(rx_data, 0, sizeof(rx_data));

	__HAL_UART_FLUSH_RDRREGISTER(&HUART_ESP8266);
	__HAL_UART_CLEAR_OREFLAG(&HUART_ESP8266);

	if (data_size_correct) {
		HAL_UART_Receive_DMA(&HUART_ESP8266, (uint8_t*) rx_data, data_size);
		HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_SET);
		tickstart = HAL_GetTick();
		while (!dma_tranfer_cplt) {
			if ((HAL_GetTick() - tickstart) > REQUEST_RESPONSE_TIME_MAX) {
				break;
			};
		}
		dma_tranfer_cplt = false;
	} else {
		HAL_GPIO_WritePin(ESP8266_REQ_GPIO_Port, ESP8266_REQ_Pin, GPIO_PIN_SET);
	}
}

void readWiFiWeatherData() {
	esp8266_requestDataSize();
	esp8266_requestData();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	dma_tranfer_cplt = true;
}
