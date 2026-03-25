#include "adc.h"

SPI_HandleTypeDef hspi1;

ADS131M08 adc(ADC_CS_GPIO_Port, ADC_CS_Pin);

int16_t adc_readings_buffer[ADC_AVERAGE_COUNT][8] = {0};
int16_t adc_tare[8] = {0};
uint16_t adc_readings_index = 0;

uint16_t adc_calibration_multiply_factor[8] =
{
	ADC_CH0_MF, ADC_CH1_MF, ADC_CH2_MF, ADC_CH3_MF, ADC_CH4_MF, ADC_CH5_MF, ADC_CH6_MF, ADC_CH7_MF
};

uint16_t adc_calibration_divider[8] =
{
	ADC_CH0_D, ADC_CH1_D, ADC_CH2_D, ADC_CH3_D, ADC_CH4_D, ADC_CH5_D, ADC_CH6_D, ADC_CH7_D
};

void adc_init()
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = ADC_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(ADC_RST_GPIO_Port, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ADC_RST_GPIO_Port, ADC_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(ADC_RESET_DELAY);
	HAL_GPIO_WritePin(ADC_RST_GPIO_Port, ADC_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(ADC_RESET_DELAY);
	HAL_GPIO_WritePin(ADC_RST_GPIO_Port, ADC_RST_Pin, GPIO_PIN_SET);

	/*Configure SPI1 */
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	    Error_Handler();

	if(adc_check_presense())
	{
		adc_setup();
		adc_fill_buffer();
		adc_set_tare();
		adc_set_OCAL();
		adc_fill_buffer();
		adc_set_tare();
	}
}

bool adc_check_presense()
{
	bool presense = 0;

	uint16_t reset = adc.reset_24();
	uint16_t standby = adc.standby_24();
	uint16_t wakeup = adc.wakeup_24();
	if(reset == 0xff28 && standby == 0x22 && wakeup == 0x33)
		presense = 1;

	return presense;
}

void adc_setup()
{
	uint16_t mode = adc.readReg_24(ADS131_MODE);			// read MODE register
	mode &= ~((1UL << 9) | (1UL << 8) | (1UL << 4));		// clear bits 9, 8 and 4 to set 16 bits mode and disable SPI timeout
	adc.writeReg_24(ADS131_MODE, mode);						// modify MODE register
	for(uint8_t ch_num = 0; ch_num < 8; ch_num++)
		adc.setGain(ch_num, ADC_GAIN);						// set GAIN defined in adc.h
	adc.setOSR(ADC_OSR);									// set OSR defined in adc.h
}

void adc_get_readings()
{
	while((adc.get_status() & ADC_DRDY_MASK) != ADC_DRDY_MASK);
	adc.getData(adc_readings_buffer[adc_readings_index]);
	adc_readings_index = (adc_readings_index + 1) % ADC_AVERAGE_COUNT;
}

int16_t adc_get_readings_average(uint8_t ch_num)
{
	int32_t sum = 0;

	for(uint8_t readings_num = 0; readings_num < ADC_AVERAGE_COUNT; readings_num++)
		sum += adc_readings_buffer[readings_num][ch_num];

	return (int16_t)(sum / ADC_AVERAGE_COUNT);
}

void adc_fill_buffer()
{
	for(uint8_t readings_num = 0; readings_num < ADC_AVERAGE_COUNT; readings_num++)
	{
		while((adc.get_status() & ADC_DRDY_MASK) != ADC_DRDY_MASK);
		adc.getData(adc_readings_buffer[adc_readings_index]);
		adc_readings_index = (adc_readings_index + 1) % ADC_AVERAGE_COUNT;
	}

}

void adc_set_tare()
{
	for(uint8_t ch_num = 0; ch_num < 8; ch_num++)
		adc_tare[ch_num] = adc_get_readings_average(ch_num);
}

void adc_set_OCAL()
{
	for(uint8_t channel = 0; channel < 8; channel++)
		adc.writeOCAL(channel, adc_tare[channel]);
}

void adc_set_OSR(uint16_t osr)
{
	adc.setOSR(osr);
}

