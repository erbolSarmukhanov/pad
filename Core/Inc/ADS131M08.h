#ifndef INC_ADS131M08_H_
#define INC_ADS131M08_H_

#include <cmath>
#include <stdint.h>
#include "main.h"

/* ADS131M08 commands */
#define NULL_COMMAND					0x00
#define RESET_COMMAND					0x11
#define STANDBY_COMMAND					0x22
#define WAKEUP_COMMAND					0x33
#define LOCK_COMMAND					0x555
#define UNLOCK_COMMAND					0x655

#define ADS131_ID 						0x00
#define ADS131_STATUS 					0x01
#define ADS131_MODE 					0x02
#define ADS131_CLOCK 					0x03
#define ADS131_GAIN1 					0x04
#define ADS131_GAIN2 					0x05
#define ADS131_CFG 						0x06
#define ADS131_THRSHLD_MSB 				0x07
#define ADS131_THRSHLD_LSB 				0x08
#define ADS131_CH0_CFG 					0x09
#define ADS131_CH0_OCAL_MSB 			0x0A
#define ADS131_CH0_OCAL_LSB 			0x0B
#define ADS131_CH0_GCAL_MSB 			0x0C
#define ADS131_CH0_GCAL_LSB 			0x0D
#define ADS131_CH1_CFG 					0x0E
#define ADS131_CH1_OCAL_MSB 			0x0F
#define ADS131_CH1_OCAL_LSB 			0x10
#define ADS131_CH1_GCAL_MSB 			0x11
#define ADS131_CH1_GCAL_LSB 			0x12
#define ADS131_CH2_CFG 					0x13
#define ADS131_CH2_OCAL_MSB 			0x14
#define ADS131_CH2_OCAL_LSB 			0x15
#define ADS131_CH2_GCAL_MSB 			0x16
#define ADS131_CH2_GCAL_LSB 			0x17
#define ADS131_CH3_CFG 					0x18
#define ADS131_CH3_OCAL_MSB 			0x19
#define ADS131_CH3_OCAL_LSB 			0x1A
#define ADS131_CH3_GCAL_MSB 			0x1B
#define ADS131_CH3_GCAL_LSB 			0x1C
#define ADS131_CH4_CFG 					0x1D
#define ADS131_CH4_OCAL_MSB 			0x1E
#define ADS131_CH4_OCAL_LSB 			0x1F
#define ADS131_CH4_GCAL_MSB 			0x20
#define ADS131_CH4_GCAL_LSB 			0x21
#define ADS131_CH5_CFG 					0x22
#define ADS131_CH5_OCAL_MSB 			0x23
#define ADS131_CH5_OCAL_LSB 			0x24
#define ADS131_CH5_GCAL_MSB 			0x25
#define ADS131_CH5_GCAL_LSB 			0x26
#define ADS131_CH6_CFG 					0x27
#define ADS131_CH6_OCAL_MSB 			0x28
#define ADS131_CH6_OCAL_LSB 			0x29
#define ADS131_CH6_GCAL_MSB 			0x2A
#define ADS131_CH6_GCAL_LSB 			0x2B
#define ADS131_CH7_CFG 					0x2C
#define ADS131_CH7_OCAL_MSB 			0x2D
#define ADS131_CH7_OCAL_LSB 			0x2E
#define ADS131_CH7_GCAL_MSB 			0x2F
#define ADS131_CH7_GCAL_LSB 			0x30
#define ADS131_REGMAP_CRC 				0x3E
#define ADS131_RESERVED 				0x3F

#define CS_DELAY						10

extern SPI_HandleTypeDef hspi1;

class ADS131M08
{
	public:
		GPIO_TypeDef* CS_GPIO = NULL;
		uint16_t CS;

		ADS131M08(GPIO_TypeDef* cs_gpio, uint16_t cs_pin);
		void cs_low();
		void cs_high();

		/******************** 24 bits mode functions ********************/
		uint16_t get_status_24();
		uint16_t reset_24();
		uint16_t standby_24();
		uint16_t wakeup_24();
		uint16_t writeReg_24(uint8_t regAddr, uint16_t regVal);
		uint16_t readReg_24(uint8_t regAddr);

		/******************** 16 bits mode functions ********************/
		uint16_t get_status();
		uint16_t reset();
		uint16_t standby();
		uint16_t wakeup();
		uint16_t lock();
		uint16_t unlock();
		uint16_t enableChannel(uint8_t channel);
		uint16_t disableChannel(uint8_t channel);
		uint16_t setGain(uint8_t channel, uint16_t gain);
		void writeOCAL(uint8_t channel, int16_t ocal);
		void writeGCAL(uint8_t channel, int16_t gcal);
		uint16_t setOSR(uint16_t osr);
		uint16_t globalChop(bool enabled, uint8_t log2delay);
		void getData(int16_t inputArr[8]);
		void readAllChannels(int16_t inputArr[8]);
		int16_t twoCompDeco(uint16_t data);
		void spiCommFrame(uint16_t* outPtr, uint16_t command = 0x00);
		uint16_t spiTransferWord(uint16_t sendWord = 0x00);
		uint16_t writeReg(uint8_t regAddr, uint16_t regVal);
		uint16_t readReg(uint8_t regAddr);
};

#endif /* INC_ADS131M08_H_ */
