#include "rs485.h"

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
uint8_t rs485_rx_buffer[RS485_BUFFER_SIZE] = {0};
volatile bool rs485_send_frame_flag = 0;

void rs485_rx_callback(uint16_t Size)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rs485_rx_buffer, RS485_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
	if (Size == sizeof(T_RS485_Command))
	{
		T_RS485_Command command;
		memcpy(&command, rs485_rx_buffer, sizeof(T_RS485_Command));
		rs485_parse_command(&command);
	}
}

void rs485_init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	rs485_dma_init();

	/*Configure GPIO pins : RS485_EN_RX_Pin RS485_EN_TX_Pin */
	GPIO_InitStruct.Pin = RS485_EN_RX_Pin|RS485_EN_TX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	RS485_SET_RX_MODE;

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	    Error_Handler();
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
		Error_Handler();
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	    Error_Handler();
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	    Error_Handler();
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rs485_rx_buffer, RS485_BUFFER_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

void rs485_dma_init()
{
	__HAL_RCC_DMA1_CLK_ENABLE();
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void rs485_parse_command(T_RS485_Command* cmd)
{
	uint8_t* crc_start = (uint8_t*)&cmd->address;
    uint8_t* crc_end = (uint8_t*)&cmd->crc;
    size_t crc_len = crc_end - crc_start;
	uint16_t calculated_crc = crc16(crc_start, crc_len);

	if(cmd->header == RS485_HEADER_CENTRAL && calculated_crc == cmd->crc)
	{
		if(cmd->address == pad_address || cmd->address == RS485_BROADCAST_ADDRESS)
		{
			switch(cmd->command)
			{
				case CMD_GET_FRAME:
					rs485_send_frame_flag = 1;
				break;
			}
		}
	}
	else
	{
		rs485_send_nack(cmd->command, ERR_CRC);
	}
}

void rs485_send_frame(T_RS485_Frame* frame)
{
	uint8_t* crc_start = (uint8_t*)&frame->address;
    uint8_t* crc_end = (uint8_t*)&frame->crc;
    size_t crc_len = crc_end - crc_start;
	uint16_t calculated_crc = crc16(crc_start, crc_len);
	frame->crc = calculated_crc;

	RS485_SET_TX_MODE;
    HAL_UART_Transmit(&huart1, (uint8_t*)frame, sizeof(T_RS485_Frame), 50);
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
    RS485_SET_RX_MODE;
}

void rs485_send_ack(uint8_t cmd)
{
	T_RS485_Ack ack =
	{
		.header = RS485_HEADER_PAD,
		.address = pad_address,
		.command = cmd,
		.ack_code = RS485_ACK,
	};

	uint8_t* crc_start = (uint8_t*)&ack.address;
    uint8_t* crc_end = (uint8_t*)&ack.crc;
    size_t crc_len = crc_end - crc_start;
    uint16_t crc = crc16(crc_start, crc_len);
    ack.crc = crc;

    RS485_SET_TX_MODE;
    HAL_UART_Transmit(&huart1, (uint8_t*)&ack, sizeof(T_RS485_Ack), 50);
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
    RS485_SET_RX_MODE;
}

void rs485_send_nack(uint8_t cmd, uint16_t err_code)
{
	T_RS485_Nack nack =
	{
		.header = RS485_HEADER_PAD,
		.address = pad_address,
		.command = cmd,
		.nack_code = RS485_NACK,
		.error_code = err_code
	};

	uint8_t* crc_start = (uint8_t*)&nack.address;
    uint8_t* crc_end = (uint8_t*)&nack.crc;
    size_t crc_len = crc_end - crc_start;
    uint16_t crc = crc16(crc_start, crc_len);
    nack.crc = crc;

    RS485_SET_TX_MODE;
    HAL_UART_Transmit(&huart1, (uint8_t*)&nack, sizeof(T_RS485_Nack), 50);
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) == RESET);
    RS485_SET_RX_MODE;
}

uint16_t crc16(uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; ++i)
    {
        crc ^= (uint16_t)(data[i]) << 8;

        for (uint8_t bit = 0; bit < 8; ++bit)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}
