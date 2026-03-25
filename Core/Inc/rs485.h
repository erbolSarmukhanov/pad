#ifndef INC_RS485_H_
#define INC_RS485_H_

#include <string.h>
#include "main.h"

#define RS485_SET_RX_MODE				HAL_GPIO_WritePin(RS485_EN_TX_GPIO_Port, RS485_EN_TX_Pin | RS485_EN_RX_Pin, GPIO_PIN_RESET)
#define RS485_SET_TX_MODE				HAL_GPIO_WritePin(RS485_EN_TX_GPIO_Port, RS485_EN_TX_Pin | RS485_EN_RX_Pin, GPIO_PIN_SET)

#define RS485_BUFFER_SIZE				128

#define RS485_HEADER_CENTRAL			0xFEED
#define RS485_HEADER_PAD				0xBEEF
#define RS485_BROADCAST_ADDRESS			0xFF

#define RS485_ACK         				0x06
#define RS485_NACK        				0x15

typedef struct __attribute__((packed))
{
	uint16_t header;
	uint8_t address;
	int16_t forceX;
	int16_t forceY;
	int16_t forceZ;
  	uint16_t crc;
} T_RS485_Frame;

typedef struct __attribute__((packed))
{
	uint16_t header;
	uint8_t address;
	uint8_t command;
  	uint16_t crc;
} T_RS485_Command;

typedef struct __attribute__((packed))
{
	uint16_t header;
	uint8_t address;
	uint8_t command;
	uint8_t ack_code;
	uint16_t crc;
} T_RS485_Ack;

typedef struct __attribute__((packed))
{
	uint16_t header;
	uint8_t address;
	uint8_t command;
	uint8_t nack_code;
	uint16_t error_code;
	uint16_t crc;
} T_RS485_Nack;

typedef enum
{
	CMD_START_STREAM = 0x01,
	CMD_STOP_STREAM = 0x05,
	CMD_GET_FRAME = 0x06,
} ECommandCode;

typedef enum
{
	ERR_PLATFORM_NOT_EXIST = 1,
  	ERR_PLATFORM_NO_RESPONSE = 2,
 	ERR_INVALID_PARAMETER = 3,
 	ERR_SELF_TEST_FAILED = 4,
 	ERR_WIFI_CONNECTION_FAILED = 5,
 	ERR_WIFI_AP_START_FAILED = 6,
	ERR_UNKNOWN_COMMAND = 7,
 	ERR_UNEFFECTIVE_COMMAND = 8,
 	ERR_CRC = 9
} T_RxError;

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern uint8_t rs485_rx_buffer[RS485_BUFFER_SIZE];
extern volatile bool rs485_send_frame_flag;
extern uint8_t pad_address;

void rs485_rx_callback(uint16_t Size);
void rs485_init();
void rs485_dma_init();
void rs485_parse_command(T_RS485_Command* cmd);
void rs485_send_frame(T_RS485_Frame* frame);
void rs485_send_ack(uint8_t cmd);
void rs485_send_nack(uint8_t cmd, uint16_t err_code);
uint16_t crc16(uint8_t *data, size_t length);

#endif /* INC_RS485_H_ */
