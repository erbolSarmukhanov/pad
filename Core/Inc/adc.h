#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <math.h>
#include "ADS131M08.h"

#define ADC_AVERAGE_COUNT					15
#define ADC_DRDY_MASK						0xFF
#define ADC_GAIN							128				// see GAIN1/GAIN2 registers
#define ADC_OSR								1024			// see bits 4:2 of the CLOCK register and Table 8-2
#define ADC_RESET_DELAY						1000

#define	ADC_CH0_MF							45
#define	ADC_CH0_D							10
#define	ADC_CH1_MF							100
#define	ADC_CH1_D							10
#define	ADC_CH2_MF							100
#define	ADC_CH2_D							10
#define	ADC_CH3_MF							1
#define	ADC_CH3_D							1
#define	ADC_CH4_MF							100
#define	ADC_CH4_D							10
#define	ADC_CH5_MF							100
#define	ADC_CH5_D							10
#define	ADC_CH6_MF							45
#define	ADC_CH6_D							10
#define	ADC_CH7_MF							1
#define	ADC_CH7_D							1

#define ADC_FZ_CHANNELS_MASK				ch_num == 1 || ch_num == 2 || ch_num == 4 || ch_num == 5

extern ADS131M08 adc;
extern int16_t adc_readings_buffer[ADC_AVERAGE_COUNT][8];
extern int16_t adc_tare[8];
extern uint16_t adc_calibration_multiply_factor[8], adc_calibration_divider[8];

void adc_init();
bool adc_check_presense();
void adc_setup();
void adc_get_readings();
int16_t adc_get_readings_average(uint8_t ch_num);
void adc_fill_buffer();
void adc_set_tare();
void adc_set_OCAL();
void adc_set_OSR(uint16_t osr);

#endif /* INC_ADC_H_ */
