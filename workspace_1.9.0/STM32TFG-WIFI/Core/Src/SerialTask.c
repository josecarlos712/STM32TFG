#include "SerialTask.h"

// Transmite un carácter a través de UART1 y UART4
// Parámetros:
// - ch: carácter a transmitir
// Retorna el carácter transmitido
PUTCHAR_PROTOTYPE {
	// Transmits character ch over UART1 and UART4
	while (HAL_OK != HAL_UART_Transmit_IT(&huart1, (uint8_t*) &ch, 1)) {
		// Wait for transmit to finish
		;
	}
	while (HAL_OK != HAL_UART_Transmit_IT(&huart4, (uint8_t*) &ch, 1)) {
		// Wait for transmit to finish
		;
	}
	return ch;
}

int printTransmision = 0;

// Tarea que se encarga de gestionar las peticiones al servidor usando una cola como elemento sincronizador
void serialRxTask(void *parg) {
	uint8_t ins = 0;
	uint8_t time = 20;

	osMessageQueueReset(instructionQueueHandle);
	while (1) {
		uint32_t messageCount = osMessageQueueGetCount(instructionQueueHandle);
		vTaskDelay(500);
		//S_PrintOnSerial("messagesWaiting %u\r\n", messageCount);
		//Si se ha consumido el mensaje significa que se ha hecho la peticion JSON
		if (messageCount < 1) {
			//S_PrintOnSerial("-new\r\n");
			// Allocate memory for a MovementInstruction_t struct
			MovementInstruction_t *pMov = malloc(sizeof(MovementInstruction_t));
			// Create the struct to be sent to the queue
			I_CreateInstructionStruct(ins, time, pMov);
			// Se envia un nuevo mensaje a la cola para que se haga otra peticion
			osStatus_t status = osMessageQueuePut(instructionQueueHandle, pMov,
					0U, 0U);
			if (status == osOK) {
				// El mensaje se envió correctamente
				// Update the instruction to be sent
				ins = (ins + 1) % I_NUM_INSTRUCTIONS;
			}
		}
	}
}

// Tarea que se encarga de enviar datos por el puerto serie.
// Genera un JSON con instrucciones y las envía.
void serialTxTask(void *parg) {
	//Simulacion de un JSON proveniente de la API para no tener que esperar la conexion cada vez
	const char *json =
			"{\"ins\": [2, 4, 0, 1, 5, 3, 1, 2, 4, 0, 5, 3, 2, 1, 4, 0, 3, 5, 2, 1, 4, 0, 3, 5, 2, 4, 0, 1, 5, 3],"
					"\"dur\": [80, 40, 70, 30, 90, 50, 10, 20, 60, 80, 40, 70, 30, 90, 50, 10, 20, 60, 80, 40, 60, 30, 90, 50, 10, 20, 60, 80, 40, 70]}";
	uint8_t iJson[MAX_NUM_INSTRUCTIONS];
	uint8_t dJson[MAX_NUM_INSTRUCTIONS];
	uint8_t *iSize = malloc(sizeof(uint8_t));
	uint8_t *dSize = malloc(sizeof(uint8_t));

	//Procesa el JSON a un par de arrays correspondiente a las instrucciones
	I_JsonToInstructions(json, iJson, iSize, dJson, dSize);

	char result[200];
	memset(result, '\0', 200 - 1);

	HAL_UART_Abort(&huart4);
	__HAL_UART_CLEAR_IT(&huart4, UART_CLEAR_OREF);

	for (size_t i = 0; i < (*iSize) && i < 20; i++) {
		// Concatenar el elemento del primer array
		sprintf(result + strlen(result), "%d %d\n", (int) iJson[i],
				(int) dJson[i]);
	}

	//sprintf(result+strlen(result), "\r\n");
	//Imprime por puerto serie las instrucciones
	S_PrintOnSerial(result);

	//printf("-------- end instructions ------\r\n");

	free(iSize);
	free(dSize);
}

// Tarea del servidor web
void WebServerTask(void *pArg) {

	//Inicia el proceso de peticiones iteradas y se detiene si hay un error en la comunicacion, conexion o recepcion
	if (RequestJSONProcess() != 0)
		S_PrintOnSerial("Se ha detenido el proceso de peticiones");

	while (1)
		vTaskDelay(1000);
}

// Crea los objetos de comunicación serie
void CreateSerialObjects() {
	// Create a queue with 16 elements, each element has a size of 1 byte
	instructionQueueHandle = xQueueCreate(16, 1);
}

// Crea las tareas de comunicación serie
void CreateSerialTask() {
	xTaskCreate(WebServerTask, "TareaWebServer", 256, NULL, 1, NULL);
	xTaskCreate(serialTxTask, "serialTxTask", 256, NULL, 1, NULL);
	xTaskCreate(serialRxTask, "serialRxTask", 256, NULL, 2, NULL);
}
