#include "ADS131M08.h"

/**
  * @brief  Enable clock, configure CS pin as output and set it high
  * @param  no parameters
  * @retval no return
  */
ADS131M08::ADS131M08(GPIO_TypeDef* cs_gpio, uint16_t cs_pin)
{
	CS_GPIO = cs_gpio;
	CS = cs_pin;

	/* Enable CS_GPIO clock */
	if(CS_GPIO == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if(CS_GPIO == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	else if(CS_GPIO == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	else if(CS_GPIO == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();

	/* Configure CS pin */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = CS;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;									// mode is push-pull output
	GPIO_InitStruct.Pull = GPIO_PULLUP;											// pull-up pin
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;								// speed is high
	HAL_GPIO_Init(CS_GPIO, &GPIO_InitStruct);									// configure CS pins
	cs_high();																	// set CS pin high
}

/**
  * @brief  Reset CS pin
  */
void ADS131M08::cs_low()
{
	HAL_GPIO_WritePin(CS_GPIO, CS, GPIO_PIN_RESET);
	for(uint16_t delay = 0; delay < CS_DELAY; delay++) __NOP();
}

/**
  * @brief  Set CS pin
  */
void ADS131M08::cs_high()
{
	for(uint16_t delay = 0; delay < CS_DELAY; delay++) __NOP();
	HAL_GPIO_WritePin(CS_GPIO, CS, GPIO_PIN_SET);
}


/************************************ 24 bits mode functions ************************************/
/**
  * @brief  Get ADC status
  * @param  no parameters
  * @retval STATUS register value
  */
uint16_t ADS131M08::get_status_24()
{
	uint16_t response = 0;
	uint8_t tx[30] = {0}, rx[30] = {0};

	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 30, 30);
	cs_high();

	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 30, 30);
	cs_high();

	response = rx[0] << 8;
	response |= rx[1];

	return response;
}

/**
  * @brief  Reset ADC
  * @param  no parameters
  * @retval RESET command response 0xff28
  */
uint16_t ADS131M08::reset_24()
{
	uint16_t response = 0;
	uint8_t tx[30] = {0}, rx[30] = {0};
	tx[1] = RESET_COMMAND;

	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 30, 30);
	cs_high();
	for(uint32_t i = 0; i < 10000; i++) __NOP();

	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 30, 30);
	cs_high();

	response = rx[0] << 8;
	response |= rx[1];

	return response;
}

/**
  * @brief  Standby ADC
  * @param  no parameters
  * @retval STANDBY command response 0x22
  */
uint16_t ADS131M08::standby_24()
{
	uint16_t response = 0;
	uint8_t tx[30] = {0}, rx[30] = {0};
	tx[1] = STANDBY_COMMAND;

	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 30, 30);
	cs_high();

	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 30, 30);
	cs_high();

	response = rx[0] << 8;
	response |= rx[1];

	return response;
}

/**
  * @brief  Wake-up ADC
  * @param  no parameters
  * @retval WAKEPUP command response 0x33
  */
uint16_t ADS131M08::wakeup_24()
{
	uint16_t response = 0;
	uint8_t tx[30] = {0}, rx[30] = {0};
	tx[1] = WAKEUP_COMMAND;

	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 30, 30);
	cs_high();

	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 30, 30);
	cs_high();

	response = rx[0] << 8;
	response |= rx[1];

	return response;
}

/**
  * @brief  Write register
  * @param  Register address
  * @param  Register value
  * @retval Successful/failure write response
  */
uint16_t ADS131M08::writeReg_24(uint8_t regAddr, uint16_t regVal)
{
    uint8_t commandPref = 0x06;													// WRITE command prefix
    uint16_t commandWord = (commandPref << 12) + (regAddr << 7);				// combine command prefix and register's address

    uint8_t tx[6] = {0}, rx[6] = {0};
    tx[0] = commandWord >> 8;
    tx[1] = commandWord;
    tx[3] = regVal >> 8;
    tx[4] = regVal;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 6, 1);
	cs_high();

	tx[0] = 0;
	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 3, 1);
	cs_high();

	uint16_t response = rx[0] << 8;
	response |= rx[1];

	return response;
}

/**
  * @brief  Read register
  * @param  Register address
  * @retval Successful/failure read response
  */
uint16_t ADS131M08::readReg_24(uint8_t regAddr)
{
    uint8_t commandPref = 0x0A;													// READ command prefix
    uint16_t commandWord = (commandPref << 12) + (regAddr << 7);				// combine command prefix and register's address

    uint8_t tx[6] = {0}, rx[6] = {0};
    tx[0] = commandWord >> 8;
    tx[1] = commandWord;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 6, 1);
	cs_high();

	tx[0] = 0;
	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 3, 1);
	cs_high();

	uint16_t regVal = rx[0] << 8;
	regVal |= rx[1];

	return regVal;
}

/************************************ 16 bits mode functions ************************************/
/**
  * @brief  Get ADC status
  * @param  no parameters
  * @retval STATUS register value
  */
uint16_t ADS131M08::get_status()
{
	uint8_t tx[20] = {0}, rx[20] = {0};
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 20, 20);							// send empty word
	cs_high();

	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 20, 20);							// send empty word to get response
	cs_high();
	uint16_t response = rx[0] << 8;
	response |= rx[1];

	return response;															// return response
}

/**
  * @brief  Reset ADC
  * @param  no parameters
  * @retval RESET command response 0xff28
  */
uint16_t ADS131M08::reset()
{
	uint8_t tx[20] = {0}, rx[20] = {0};
	tx[1] = RESET_COMMAND;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 20, 20);										// send RESET command
	cs_high();
	for(uint32_t i = 0; i < 10000; i++) __NOP();

	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 20, 20);							// send empty word to get response
	cs_high();
	uint16_t response = rx[0] << 8;
	response |= rx[1];

	return response;															// return response
}

/**
  * @brief  Standby ADC
  * @param  no parameters
  * @retval STANDBY command response 0x22
  */
uint16_t ADS131M08::standby()
{
	uint8_t tx[20] = {0}, rx[20] = {0};
	tx[1] = STANDBY_COMMAND;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 20, 20);										// send STANDBY command
	cs_high();

	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 20, 20);							// send empty word to get response
	cs_high();
	uint16_t response = rx[0] << 8;
	response |= rx[1];

	return response;															// return response
}

/**
  * @brief  Wake-up ADC
  * @param  no parameters
  * @retval WAKEPUP command response 0x33
  */
uint16_t ADS131M08::wakeup()
{
	uint8_t tx[20] = {0}, rx[20] = {0};
	tx[1] = WAKEUP_COMMAND;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 20, 20);										// send WAKEUP command
	cs_high();

	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 20, 20);							// send empty word to get response
	cs_high();
	uint16_t response = rx[0] << 8;
	response |= rx[1];

	return response;															// return response
}

/**
  * @brief  Lock ADC interface
  * @param  no parameters
  * @retval LOCK command response 0x555
  */
uint16_t ADS131M08::lock()
{
	uint8_t tx[2] = {0}, rx[2] = {0};
	tx[0] = (uint8_t)(LOCK_COMMAND >> 8);
	tx[1] = (uint8_t)LOCK_COMMAND;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 2, 1);											// send LOCK command
	cs_high();

	tx[0] = 0;
	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 1);								// send empty word to get response
	cs_high();
	uint16_t response = rx[0] << 8;
	response |= rx[1];

	return response;															// return response
}

/**
  * @brief  Unlock ADC interface
  * @param  no parameters
  * @retval UNLOCK command response 0x655
  */
uint16_t ADS131M08::unlock()
{
	uint8_t tx[2] = {0}, rx[2] = {0};
	tx[0] = (uint8_t)(UNLOCK_COMMAND >> 8);
	tx[1] = (uint8_t)UNLOCK_COMMAND;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 2, 1);											// send UNLOCK command
	cs_high();

	tx[0] = 0;
	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 1);								// send empty word to get response
	cs_high();
	uint16_t response = rx[0] << 8;
	response |= rx[1];

	return response;															// return response
}

/**
  * @brief  Enable ADC channel
  * @param  Channel number
  * @retval Successful/failure enable response
  */
uint16_t ADS131M08::enableChannel(uint8_t channel)
{
	uint16_t clock = readReg(ADS131_CLOCK);										// read ADS131_CLOCK register
	clock |= (1UL << (channel + 8));											// modify bit needed
	uint16_t response = writeReg(ADS131_CLOCK, clock);							// write ADS131_CLOCK register

	return response;															// return response
}

/**
  * @brief  Disable channel
  * @param  Channel number
  * @retval Successful/failure disable response
  */
uint16_t ADS131M08::disableChannel(uint8_t channel)
{
	uint16_t clock = readReg(ADS131_CLOCK);										// read ADS131_CLOCK register
	clock &= ~(1UL << (channel + 8));											// modify bit needed
	uint16_t response = writeReg(ADS131_CLOCK, clock);							// write ADS131_CLOCK register

	return response;															// return response
}

/**
  * @brief  Set ADC channel gain
  * @param  Channel number
  * @param  Gain value
  * @retval Successful/failure gain set response
  */
uint16_t ADS131M08::setGain(uint8_t channel, uint16_t gain)
{
	uint16_t gainReg = 0;
	uint8_t shift = 0;
	uint16_t mask = 0;

	if(channel < 4)																// select ADS131_GAIN1 register if channel < 4
		gainReg = ADS131_GAIN1;
	else if(channel < 8)
		gainReg = ADS131_GAIN2;													// select ADS131_GAIN2 register if channel is 4 to 7
	else
		return false;															// return false if channel's number out of range

	if(channel == 3 || channel == 7)
		shift = 12;																// PGAGAIN_0 bit is 12 for 3rd and 7th channels
	else if(channel == 2 || channel == 6)
		shift = 8;																// PGAGAIN_0 bit is 8 for 2nd and 6th channels
	else if(channel == 1 || channel == 5)
		shift = 4;																// PGAGAIN_0 bit is 4 for 1st and 5th channels
	else if(channel == 0 || channel == 4)
		shift = 0;																// PGAGAIN_0 bit is 0 for 0 and 4th channels
	else
		return false;															// return false if channel's number out of range

	if(gain == 1)																// select mask for the PGAGAIN bits
		mask = 0b000;
	else if(gain == 2)
		mask = 0b001UL;
	else if(gain == 4)
		mask = 0b010UL;
	else if(gain == 8)
		mask = 0b011UL;
	else if(gain == 16)
		mask = 0b100UL;
	else if(gain == 32)
		mask = 0b101UL;
	else if(gain == 64)
		mask = 0b110UL;
	else if(gain == 128)
		mask = 0b111UL;
	else
		return false;															// return false if gain's value is not valid

	uint16_t regValue = readReg(gainReg);										// read GAIN register
	regValue &= ~(0b111UL << shift);											// clear bits required
	regValue |= (mask << shift); 												// modify bits required
	uint16_t response = writeReg(gainReg, regValue);							// write modified value to the GAIN register

	return response;
}

/**
  * @brief  Write OCAL
  * @param  Channel number
  * @param  OCAL value
  * @retval no return
  */
void ADS131M08::writeOCAL(uint8_t channel, int16_t ocal)
{
	uint8_t ocalRegMSB = 0;

	switch(channel)																// switch OCAL_MSB and OCAL_LSB registers
	{
		case 0:
			ocalRegMSB = ADS131_CH0_OCAL_MSB;
		break;

		case 1:
			ocalRegMSB = ADS131_CH1_OCAL_MSB;
		break;

		case 2:
			ocalRegMSB = ADS131_CH2_OCAL_MSB;
		break;

		case 3:
			ocalRegMSB = ADS131_CH3_OCAL_MSB;
		break;

		case 4:
			ocalRegMSB = ADS131_CH4_OCAL_MSB;
		break;

		case 5:
			ocalRegMSB = ADS131_CH5_OCAL_MSB;
		break;

		case 6:
			ocalRegMSB = ADS131_CH6_OCAL_MSB;
		break;

		case 7:
			ocalRegMSB = ADS131_CH7_OCAL_MSB;
		break;

		default:
		break;
	}

	writeReg(ocalRegMSB, ocal);
}

/**
  * @brief  Writes GCAL
  * @param  Channel number
  * @param  GCAL value
  * @retval no return
  */
void ADS131M08::writeGCAL(uint8_t channel, int16_t gcal)
{
	uint8_t gcalRegMSB = 0, gcalRegLSB = 0;

	switch(channel)																// switch GCAL_MSB and GCAL_LSB registers
	{
		case 0:
			gcalRegMSB = ADS131_CH0_GCAL_MSB;
			gcalRegLSB = ADS131_CH0_GCAL_LSB;
		break;

		case 1:
			gcalRegMSB = ADS131_CH1_GCAL_MSB;
			gcalRegLSB = ADS131_CH1_GCAL_LSB;
		break;

		case 2:
			gcalRegMSB = ADS131_CH2_GCAL_MSB;
			gcalRegLSB = ADS131_CH2_GCAL_LSB;
		break;

		case 3:
			gcalRegMSB = ADS131_CH3_GCAL_MSB;
			gcalRegLSB = ADS131_CH3_GCAL_LSB;
		break;

		case 4:
			gcalRegMSB = ADS131_CH4_GCAL_MSB;
			gcalRegLSB = ADS131_CH4_GCAL_LSB;
		break;

		case 5:
			gcalRegMSB = ADS131_CH5_GCAL_MSB;
			gcalRegLSB = ADS131_CH5_GCAL_LSB;
		break;

		case 6:
			gcalRegMSB = ADS131_CH6_GCAL_MSB;
			gcalRegLSB = ADS131_CH6_GCAL_LSB;
		break;

		case 7:
			gcalRegMSB = ADS131_CH7_GCAL_MSB;
			gcalRegLSB = ADS131_CH7_GCAL_LSB;
		break;

		default:
		break;
	}

	writeReg(gcalRegMSB, (uint16_t)(gcal >> 8));								// write GCAL_MSB register
	writeReg(gcalRegLSB, (uint16_t)(gcal << 8));								// write GCAL_LSB register
}

/**
  * @brief  Set OSR
  * @param  OSR value
  * @retval Successful/failure set response
  */
uint16_t ADS131M08::setOSR(uint16_t osr)
{
	uint16_t response = 0;
	uint16_t mask = 0;

	if(osr == 128)
		mask = 0UL;
	else if(osr == 256)
		mask = 1UL;
	else if(osr == 512)
		mask = 2UL;
	else if(osr == 1024)
		mask = 3UL;
	else if(osr == 2048)
		mask = 4UL;
	else if(osr == 4096)
		mask = 5UL;
	else if(osr == 8192)
		mask = 6UL;
	else if(osr == 16256)
		mask = 7UL;
	else
		return response;

	uint16_t clock = readReg(ADS131_CLOCK);
	clock &= ~((1UL << 4) | (1UL << 3) | (1UL << 2));
	clock |= mask << 2;
	response = writeReg(ADS131_CLOCK, clock);

	return response;
}

/**
  * @brief  Enable/disable global chop
  * @param  1- enable, 0 - disable
  * @param  Delay value from 1 to 16
  * @retval Successful/failure set response
  */
uint16_t ADS131M08::globalChop(bool enabled, uint8_t log2delay)
{
    uint8_t delayRegData = log2delay - 1;
    uint16_t currentDetSett = (readReg(ADS131_CFG) << 8) >> 8;
    uint16_t newRegData = (delayRegData << 12) + (enabled << 8) + currentDetSett;

    return writeReg(ADS131_CFG, newRegData);
}

/**
  * @brief  Save the decoded data for each of the channels
  * @param  Array to save decoded data
  * @retval no return
  */
void ADS131M08::getData(int16_t inputArr[8])
{
	uint8_t tx[20] = {0}, rx[20] = {0};
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 20, 1);
	cs_high();

    for (int i = 0; i < 8; i++)
	{
        uint16_t raw = (rx[2 + i*2] << 8) | rx[3 + i*2];
        inputArr[i] = twoCompDeco(raw);
    }
}

/**
  * @brief  Save the decoded data for each of the channels
  * @param  Array to save decoded data for each of the channels
  * @retval no return
  */
void ADS131M08::readAllChannels(int16_t inputArr[8])
{
    uint16_t rawDataArr[10];
    int8_t channelArrPtr = 0;

    // Get data
    spiCommFrame(&rawDataArr[0]);
    // Save the decoded data for each of the channels
    for (int8_t i = 0; i<8; i++)
    {
        inputArr[i] = twoCompDeco(rawDataArr[channelArrPtr+1]);
        channelArrPtr++;
    }
}

/**
  * @brief  Takes the two's complement of the data
  * @param  Data to be convert
  * @retval Converted data
  */
int16_t ADS131M08::twoCompDeco(uint16_t data)
{
    if (data & 0x8000)															// Check if the number is negative (MSB is 1)
        return (int16_t)(data | 0xFFFF0000);  									// Convert to negative two's complement value

    return (int16_t)data;  														// Positive value, no change needed
}

/**
  * @brief  Save all the data of a communication frame to an array pointed by outPtr
  * @param  16-bit pointer to an array
  * @param  Command sent, default is 0x00
  * @retval no return
  */
void ADS131M08::spiCommFrame(uint16_t* outPtr, uint16_t command)
{
    cs_low();																	// reset CS pin
    *outPtr = spiTransferWord(command);											// send command and receive previous frame's response
    for (uint8_t i = 1; i < 9; i++)												// send next 8 words and save received words to an array
    {
        outPtr++;
        *outPtr = spiTransferWord();
    }

    outPtr++;
    *outPtr = spiTransferWord();												// send empty as frame's 10th word
    cs_high();																	// set CS pin
}

/**
  * @brief  Send and receive 16-bit word
  * @param  16-bit word sent
  * @retval 24-bit word received
  */
uint16_t ADS131M08::spiTransferWord(uint16_t sendWord)
{
	uint8_t tx = 0, rx = 0;
	uint16_t receivedWord = 0;

    tx = sendWord >> 8;
    HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, 1);
    receivedWord = rx << 8;
    tx = sendWord;
    HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, 1);
    receivedWord |= rx;

    return receivedWord;														// return received word
}

/**
  * @brief  Write register
  * @param  Register address
  * @param  Register value
  * @retval Successful/failure write response
  */
uint16_t ADS131M08::writeReg(uint8_t regAddr, uint16_t regVal)
{
    uint8_t commandPref = 0x06;													// WRITE command prefix
    uint16_t commandWord = (commandPref << 12) + (regAddr << 7);				// combine command prefix and register's address

    uint8_t tx[4] = {0}, rx[4] = {0};
    tx[0] = commandWord >> 8;
    tx[1] = commandWord;
    tx[2] = regVal >> 8;
    tx[3] = regVal;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 4, 1);
	cs_high();

	tx[0] = 0;
	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 1);
	cs_high();

	uint16_t response = rx[0] << 8;
	response |= rx[1];

	return response;
}

/**
  * @brief  Read register
  * @param  Register address
  * @retval Successful/failure read response
  */
uint16_t ADS131M08::readReg(uint8_t regAddr)
{
    uint8_t commandPref = 0x0A;													// READ command prefix
    uint16_t commandWord = (commandPref << 12) + (regAddr << 7);				// combine command prefix and register's address

    uint8_t tx[4] = {0}, rx[4] = {0};
    tx[0] = commandWord >> 8;
    tx[1] = commandWord;
	cs_low();
	HAL_SPI_Transmit(&hspi1, tx, 4, 1);
	cs_high();

	tx[0] = 0;
	tx[1] = 0;
	cs_low();
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 1);
	cs_high();
	uint16_t regVal = rx[0] << 8;
	regVal |= rx[1];

	return regVal;
}
