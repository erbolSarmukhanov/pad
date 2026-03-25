#include "switch.h"

void switch_init()
{
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = SW_1_Pin|SW_2_Pin|SW_3_Pin|SW_4_Pin|SW_5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t switch_get_address()
{
	uint8_t address = 0;

	address |= (HAL_GPIO_ReadPin(SW_1_GPIO_Port, SW_1_Pin) << 0);
	address |= (HAL_GPIO_ReadPin(SW_2_GPIO_Port, SW_2_Pin) << 1);
	address |= (HAL_GPIO_ReadPin(SW_3_GPIO_Port, SW_3_Pin) << 2);
	address |= (HAL_GPIO_ReadPin(SW_4_GPIO_Port, SW_4_Pin) << 3);
	address |= (HAL_GPIO_ReadPin(SW_5_GPIO_Port, SW_5_Pin) << 4);

	return address;
}




