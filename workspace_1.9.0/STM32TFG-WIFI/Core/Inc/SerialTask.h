// Se define la macro TERMINAL_USE para usar la terminal
#define TERMINAL_USE

// Si se está usando GNU C Compiler, se definen estas macros
#ifdef __GNUC__

// Prototipo de función para enviar un carácter a través de la UART
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
// Prototipo de función para recibir un carácter a través de la UART
#define GETCHAR_PROTOTYPE int __io_getchar(void)

// Se incluyen las siguientes librerías
#include "HAL_Instructions.h"
#include "FreeRTOS.h"
#include "stm32l4xx_hal.h"
#include "cmsis_os2.h"
#include "semphr.h"

// Declara los identificadores externos de las estructuras UART_HandleTypeDef y osSemaphoreId_t, y osMessageQueueId_t
extern UART_HandleTypeDef huart4, huart1;
extern osSemaphoreId_t xSemaphoreSerialHandle;
extern osMessageQueueId_t instructionQueueHandle;

// Se declaran las siguientes funciones
void CreateSerialTask();
void CreateSerialObjects(void);
void serialRxTask(void *parg);
void serialTxTask(void *parg);
void PrintTask(void *parg);
int wifi_server(void);
int RequestJSONProcess();
void WebServerTask(void *pArg);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

// Si no se está usando GNU C Compiler, se definen estas macros
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif

// Si se está usando la terminal, se define la macro LOG para imprimir mensajes en la terminal a través de printf
#ifdef  TERMINAL_USE
#define LOG(a) printf a
// Si no se está usando la terminal, se define LOG como una macro vacía
#else
#define LOG(a)
#endif
