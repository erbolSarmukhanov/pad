#include "pad.h"

T_RS485_Frame pad_frame;
uint8_t pad_address = 0;

void pad_init()
{
	switch_init();
	pad_frame.header = 0xBEEF;
	pad_address = switch_get_address();
	pad_frame.address = pad_address;
	rs485_init();
	adc_init();
}

void pad_calculate_data()
{
	int16_t average[8] = {0};
	for(uint8_t ch_num = 0; ch_num < 8; ch_num++)
	{
		average[ch_num] = adc_get_readings_average(ch_num);
		average[ch_num] -= adc_tare[ch_num];
		if(ADC_FZ_CHANNELS_MASK)
			average[ch_num] = abs(average[ch_num]);
	}
	pad_frame.forceX = average[6];
	pad_frame.forceY = average[0];
	pad_frame.forceZ = average[1] + average[2] + average[4] + average[5];
}

void pad_control()
{
	if(rs485_send_frame_flag)
	{
		rs485_send_frame_flag = 0;
		adc_get_readings();
		pad_calculate_data();
		rs485_send_frame(&pad_frame);
	}
}



