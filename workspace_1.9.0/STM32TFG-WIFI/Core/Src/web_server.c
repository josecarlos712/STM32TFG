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
#define MAX_CHUNK_SIZE 128

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
bool RequestJSON(uint8_t *jsonbody);
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

		for (int i = 0; i < 4; i++) {
			if (WIFI_OpenClientConnection(clientSocket, WIFI_TCP_PROTOCOL, "",
					serverIPAddr, serverPort, 0) == WIFI_STATUS_OK) {
				//S_PrintOnSerial("-client\r\n");
				//Infinite bucle of API requests
				uint8_t *response = malloc(sizeof(uint8_t) * MAX_CHUNK_SIZE);

				StopServer = RequestJSON(response);
				printf("%s\r\n", (char*) response);

				free(response);
				//Disconnect from server
				WIFI_CloseClientConnection(clientSocket);
			} else {
				S_PrintOnSerial("-fail client\r\n");
			}

			//S_PrintOnSerial("-clossing client");
		}
		// Disconnect from WiFi
		WIFI_Disconnect();
		S_PrintOnSerial("-clossing wifi");
	} else {
		S_PrintOnSerial("-fail host\r\n");
	}

	return 1;
}

bool RequestJSON(uint8_t *jsonbody) {
	uint32_t clientSocket = 0;
	//Send an HTTP GET request
	const char *getRequest =
			"GET /mi-api HTTP/1.1\r\nHost: 192.168.18.3\r\n\r\n";
	uint8_t response[512]; // Adjust the response buffer size as per your requirements
	uint16_t getRequestLength = strlen(getRequest);

	// Send data in smaller chunks
	uint16_t sentDataLength = 0;
	uint16_t remainingDataLength = getRequestLength;
	while (remainingDataLength > 0) {
		uint16_t chunkSize =
				(remainingDataLength > MAX_CHUNK_SIZE) ?
				MAX_CHUNK_SIZE :
															remainingDataLength;
		if (WIFI_SendData(clientSocket,
				(uint8_t*) (getRequest + sentDataLength), chunkSize,
				&sentDataLength, WIFI_WRITE_TIMEOUT) == WIFI_STATUS_OK) {
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

					// Print the received chunk
					//printf("%s\r\n", chunk);
					memcpy(jsonbody, chunk, receivedChunkLength + 1);

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

/**
 * @brief  Send HTML page
 * @param  None
 * @retval None
 */
/*
 static WIFI_Status_t SendWebPage(uint8_t ledIsOn, uint8_t temperature) {
 uint16_t SentDataLength;
 WIFI_Status_t ret;

 // construct web page content /
 char http[512];  // Variable para almacenar la página web
 uint8_t temp[50]; // Variable para almacenar la temperatura convertida a cadena

 // Inicializar la variable http con la cabecera de la página web
 strcpy((char*) http,
 (char*) "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n");

 // Concatenar el inicio del contenido de la página web
 strcat((char*) http,
 (char*) "<html><body><title>STM32cubeide Web Server</title><h2>InventekSys: Web Server STM32</h2><br /><hr><p><form method=\"POST\"><strong>Temp: <input type=\"text\" value=\"");

 // Convertir la temperatura a una cadena de caracteres y concatenarla al contenido de la página web
 sprintf((char*) temp, "%d", temperature);
 strcat((char*) http, (char*) temp);

 // Concatenar el resto del contenido de la página web
 strcat((char*) http,
 (char*) "\"><sup>O</sup>C <input type=\"radio\" name=\"radio\" value=\"4\" checked>apagado<br><input type=\"radio\" name=\"radio\" value=\"2\">encendido</strong><p><input type=\"submit\"></form></span></p></body></html>");

 // Enviar los datos de la página web a través de la conexión Wi-Fi
 ret = WIFI_SendData(0, (uint8_t*) http, strlen((char*) http),
 &SentDataLength, WIFI_WRITE_TIMEOUT);

 // Comprobar si se enviaron todos los datos correctamente
 if ((ret == WIFI_STATUS_OK) && (SentDataLength != strlen((char*) http))) {
 ret = WIFI_STATUS_ERROR;
 }

 return ret;
 }
 */
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

/*
 static int wifi_start(void) {
 uint8_t MAC_Addr[6];

 /Initialize and use WIFI module /
 if (WIFI_Init() == WIFI_STATUS_OK) {
 //LOG(("ES-WIFI Initialized.\n\r"));
 if (WIFI_GetMAC_Address(MAC_Addr) == WIFI_STATUS_OK) {
 LOG(("> es-wifi module MAC Address : %X:%X:%X:%X:%X:%X\n\r",
 MAC_Addr[0],
 MAC_Addr[1],
 MAC_Addr[2],
 MAC_Addr[3],
 MAC_Addr[4],
 MAC_Addr[5]));/
 } else {
 //LOG(("> ERROR : CANNOT get MAC address\n\r"));
 return -1;
 }
 } else {
 return -1;
 }
 return 0;
 }

 int wifi_connect(void) {

 wifi_start();

 //LOG(("\nConnecting to %s , %s\n\r",SSID,PASSWORD));
 if (WIFI_Connect(SSID, PASSWORD, WIFI_ECN_WPA2_PSK) == WIFI_STATUS_OK) {
 if (WIFI_GetIP_Address(IP_Addr) == WIFI_STATUS_OK) {
 /LOG(("> es-wifi module connected: got IP Address : %d.%d.%d.%d\n\r",
 IP_Addr[0],
 IP_Addr[1],
 IP_Addr[2],
 IP_Addr[3]));/
 } else {
 //LOG((" ERROR : es-wifi module CANNOT get IP address\n\r"));
 return -1;
 }
 } else {
 //LOG(("ERROR : es-wifi module NOT connected\n\r"));
 return -1;
 }
 return 0;
 }

 int wifi_server(void) {
 bool StopServer = false;

 //LOG(("\nRunning HTML Server test\n\r"));
 if (wifi_connect() != 0)
 return -1;

 if (WIFI_STATUS_OK
 != WIFI_StartServer(SOCKET, WIFI_TCP_PROTOCOL, 1, "", PORT)) {
 //LOG(("ERROR: Cannot start server.\n\r"));
 //LOG(("\n\r"));
 }

 //LOG(("Server is running and waiting for an HTTP  Client connection to %d.%d.%d.%d\n\r",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));

 do {
 uint8_t RemoteIP[4];
 uint16_t RemotePort;

 while (WIFI_STATUS_OK
 != WIFI_WaitServerConnection(SOCKET, 100, RemoteIP, &RemotePort)) {
 //LOG(("Waiting connection to  %d.%d.%d.%d\n\r",IP_Addr[0],IP_Addr[1],IP_Addr[2],IP_Addr[3]));
 //LOG(("\rIDLE\n"));
 vTaskDelay(10);
 }

 //LOG(("Client connected %d.%d.%d.%d:%d\n\r",RemoteIP[0],RemoteIP[1],RemoteIP[2],RemoteIP[3],RemotePort));

 StopServer = RequestJSON();
 vTaskDelay(2000);

 if (WIFI_CloseServerConnection(SOCKET) != WIFI_STATUS_OK) {
 //LOG(("ERROR: failed to close current Server connection\n\r"));
 //LOG(("\n\r"));
 return -1;
 }
 } while (StopServer == false);

 if (WIFI_STATUS_OK != WIFI_StopServer(SOCKET)) {
 //LOG(("ERROR: Cannot stop server.\n\r"));
 //LOG(("\n\r"));
 }

 //LOG(("Server is stop\n\r"));
 return 0;
 }

 static bool WebServerProcess(void) {
 uint8_t LedState = 1;
 uint8_t temp;
 uint16_t respLen;
 static uint8_t resp[1024];
 bool stopserver = false;

 if (WIFI_STATUS_OK == WIFI_ReceiveData(SOCKET, resp, 1000, &respLen,
 WIFI_READ_TIMEOUT)) {
 //LOG(("get %d byte from server\n\r",respLen));

 if (respLen > 0) {
 if (strstr((char*) resp, "GET")) // GET: put web page /
 {
 //temp = (int) GetSensores(); //BSP_TSENSOR_ReadTemp();
 if (SendWebPage(LedState, temp) != WIFI_STATUS_OK) {
 //LOG(("> ERROR : Cannot send web page\n\r"));
 } else {
 //LOG(("Send page after  GET command\n\r"));
 }
 } else if (strstr((char*) resp, "POST"))/ POST: received info /
 {
 //LOG(("Post request\n\r"));

 if (strstr((char*) resp, "radio")) {
 if (strstr((char*) resp, "radio=0")) {
 LedState = 0;
 //EncolarLED(LedState);
 } else if (strstr((char*) resp, "radio=1")) {
 LedState = 1;
 //EncolarLED(LedState);
 }
 //temp = (int) GetSensores(); //BSP_TSENSOR_ReadTemp();
 }
 if (strstr((char*) resp, "stop_server")) {
 if (strstr((char*) resp, "stop_server=0")) {
 stopserver = false;
 } else if (strstr((char*) resp, "stop_server=1")) {
 stopserver = true;
 }
 }
 //temp = (int) GetSensores(); //BSP_TSENSOR_ReadTemp();
 if (SendWebPage(LedState, temp) != WIFI_STATUS_OK) {
 //LOG(("> ERROR : Cannot send web page\n\r"));
 } else {
 //LOG(("Send Page after POST command\n\r"));
 }
 }
 }
 } else {
 //LOG(("Client close connection\n\r"));
 }
 return stopserver;

 }
 */
