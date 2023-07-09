/*
 * Wifi.c
 *
 *  Created on: Nov 29, 2020
 *      Author: rpaz
 */

#include <unistd.h>
#include "main.h"
#include "es_wifi.h"
#include "wifi.h"
#include "cmsis_os.h"
#include "semphr.h"

extern float temperature;

/* Update SSID and PASSWORD with own Access point settings */
#define SSID     "ONSICOM_CRESPO"
#define PASSWORD "1971J1998JC2002J2009A"
#define PORT           80
#define MAX_CHUNK_SIZE 256
#define MAX_REQUEST_SIZE 128

#define TERMINAL_USE

#define WIFI_WRITE_TIMEOUT 10000
#define WIFI_READ_TIMEOUT  10000
#define SOCKET                 0
#define AT_OK_STRING "\r\nOK\r\n> "

#ifdef  TERMINAL_USE
#define LOG(a) printf a
#else
#define LOG(a)
#endif
/* Private defines -----------------------------------------------------------*/
#define MAX_RESPONSE_SIZE 1024 // Tamaño máximo del buffer para almacenar la respuesta

// Prototipos
int wifi_server(void);
//static WIFI_Status_t SendWebPage(uint8_t ledIsOn, uint8_t temperature);
//static int wifi_start(void);
//static int wifi_connect(void);
//static bool WebServerProcess(void);
bool RequestJSON(uint8_t *jsonbody, char *request);
int RequestJSONProcess();

int RequestJSONProcess() {
	bool StopServer = false;
	uint16_t serverPort = 5000;

	WIFI_Ecn_t ecn = WIFI_ECN_WPA2_PSK; // Adjust the network encryption type as per your network configuration
	char ssid[] = "ONSICOM_CRESPO"; // Replace with your WiFi SSID
	char password[] = "1971J1998JC2002J2009A"; // Replace with your WiFi password

	if (WIFI_Init() != WIFI_STATUS_OK)
		return 1;
	printf("-init\r\n");

	if (WIFI_Connect(ssid, password, ecn) == WIFI_STATUS_OK) {
		S_PrintOnSerial("-connected\r\n");

		uint8_t serverIPAddr[4] = { 192, 168, 18, 3 };
		uint32_t clientSocket = 0;

		while (!StopServer) {
			MovementInstruction_t *pMov;
			//Espera a un mensaje proveniente de la tarea serialRxTask para hacer una peticion
			osStatus_t status = osMessageQueueGet(instructionQueueHandle, &pMov,
			NULL,
			osWaitForever);
			// Receive the instruction from the queue
			if (status == osOK && pMov != NULL) {
				if (WIFI_OpenClientConnection(clientSocket, WIFI_TCP_PROTOCOL,
						"", serverIPAddr, serverPort, 0) == WIFI_STATUS_OK) {
					S_PrintOnSerial("-client\r\n");
					uint8_t response[MAX_CHUNK_SIZE];
					//Request
					char *request1 =
							"GET /mi-api HTTP/1.1\r\nHost: 192.168.18.3\r\n\r\n";
					StopServer = RequestJSON(response, request1);

					//Transformar el JSON en instrucciones
					uint8_t *instructionJson = malloc(
							sizeof(uint8_t) * MAX_NUM_INSTRUCTIONS);
					uint8_t *durationJson = malloc(
							sizeof(uint8_t) * MAX_NUM_INSTRUCTIONS);
					uint8_t *iSize = malloc(sizeof(uint8_t));
					uint8_t *dSize = malloc(sizeof(uint8_t));

					I_JsonToInstructions((const char*) response,
							instructionJson, iSize, durationJson, dSize);

					char *serialJson = malloc(sizeof(char) * 200);

					for (size_t i = 0; i < (*iSize); ++i) {
						sprintf(serialJson, "%d %d\n", instructionJson[i],
								durationJson[i]);
						serialJson += strlen(serialJson); // Mueve el puntero al final de la cadena agregada
					}

					printf("%s", serialJson);

					//Disconnect from server
					WIFI_CloseClientConnection(clientSocket);
				} else {
					S_PrintOnSerial("-fail client\r\n");

				}
				free(pMov);
			}

			//S_PrintOnSerial("-clossing client");
		}
		// Disconnect from WiFi
		WIFI_Disconnect();
		S_PrintOnSerial("-clossing wifi");
	} else {
		S_PrintOnSerial("-fail host\r\n");
	}

	return (int) StopServer;
}

// Realiza una solicitud HTTP GET para obtener un JSON de un servidor remoto
// Parámetros:
// - jsonbody: puntero al array de bytes donde se almacenará el JSON recibido
// - request: cadena que representa la solicitud HTTP GET
// Retorna:
// - true si la solicitud se completó con éxito, false si ocurrió un error
bool RequestJSON(uint8_t *jsonbody, char *request) {
	uint32_t clientSocket = 0;
	//Send an HTTP GET request
	//const char *getRequest = "GET /mi-api HTTP/1.1\r\nHost: 192.168.18.3\r\n\r\n";
	uint8_t response[512]; // Adjust the response buffer size as per your requirements
	uint16_t getRequestLength = strlen(request);

	// Send data in smaller chunks
	uint16_t sentDataLength = 0;
	uint16_t remainingDataLength = getRequestLength;
	while (remainingDataLength > 0) {
		uint16_t chunkSize =
				(remainingDataLength > MAX_CHUNK_SIZE) ?
				MAX_CHUNK_SIZE :
															remainingDataLength;
		if (WIFI_SendData(clientSocket, (uint8_t*) (request + sentDataLength),
				chunkSize, &sentDataLength, WIFI_WRITE_TIMEOUT)
				== WIFI_STATUS_OK) {
			remainingDataLength -= sentDataLength;
		} else {
			printf("Failed to send the request.\n");
			break;
		}
	}

	// Receive the response
	uint16_t receivedDataLength = 0;
	uint32_t receiveTimeout = 10000; // Adjust the receive timeout value as per your requirements

	// Receive the HTTP response headers
	if (WIFI_ReceiveData(clientSocket, response, sizeof(response) - 1,
			&receivedDataLength, receiveTimeout) == WIFI_STATUS_OK) {
		response[receivedDataLength] = '\0'; // Null-terminate the response

		// Find the end of headers marker "\r\n\r\n"
		char *endOfHeaders = strstr((char*) response, "\r\n\r\n");
		if (endOfHeaders) {
			// Print the JSON body
			//printf("Received JSON body:\n");

			// Print the remaining JSON body
			//printf("%s", endOfHeaders + 4);

			// Receive and print the remaining JSON body
			while (1) {
				uint8_t chunk[MAX_CHUNK_SIZE]; // Adjust the chunk size as per your requirements
				uint16_t receivedChunkLength = 0;
				uint32_t receiveTimeout = 1000; // Adjust the receive timeout value as per your requirements

				// Receive a chunk of data
				if (WIFI_ReceiveData(clientSocket, chunk, MAX_CHUNK_SIZE - 1,
						&receivedChunkLength, receiveTimeout)
						== WIFI_STATUS_OK) {
					chunk[receivedChunkLength] = '\0'; // Null-terminate the chunk

					// Buscar la posición de la cadena "\r\n\OK\r\n\>"
					char *endMarker = strstr((char*) chunk, "\r\nOK\r\n>");

					if (endMarker != NULL) {
						// Calcular la longitud del JSON
						size_t jsonLength = endMarker - (char*) chunk;

						// Copiar el JSON a una nueva ubicación en la memoria
						memmove(chunk, chunk, jsonLength);

						// Agregar el carácter de fin de línea '\0' al final del JSON
						chunk[jsonLength] = '\0';

						//Devolver el contenido del json
						memcpy(jsonbody, chunk, jsonLength + 1);
					}

					// Check if the chunk is the end of the response
					if (receivedChunkLength < sizeof(chunk) - 1) {
						break;
					}
				} else {
					printf("Failed to receive the JSON body.\n");
					break;
				}
			}
		} else {
			printf("Failed to find the end of headers marker.\n");
		}
	} else {
		printf("Failed to receive the response headers.\n");
	}

	return 0;
}

//*************************************************************************//
extern SPI_HandleTypeDef hspi3;

/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file.                                                                     */
/************************************************************
 *															*
 *		Rutinas de servicio de interrupciones				*
 *															*
 ************************************************************/

/**
 * @brief  This function handles external lines 1interrupt request.
 * @param  None
 * @retval None
 */
void EXTI1_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
	portYIELD_FROM_ISR(pdFALSE);
}

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case (GPIO_PIN_1): {
		SPI_WIFI_ISR();
		break;
	}
	default: {
		break;
	}
	}
	portYIELD_FROM_ISR(pdFALSE);
}

/**
 * @brief  SPI3 line detection callback.
 * @param  None
 * @retval None
 */
extern SPI_HandleTypeDef hspi;
void SPI3_IRQHandler(void) {
	HAL_SPI_IRQHandler(&hspi);
	portYIELD_FROM_ISR(pdFALSE);
}
