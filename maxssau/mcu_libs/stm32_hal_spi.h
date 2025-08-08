/*
 * stm32_hal_spi.h
 *
 *  Created on: Aug 8, 2025
 *      Author: maximove
 */

#ifndef APP_STM32_HAL_SPI_H_
#define APP_STM32_HAL_SPI_H_

#include "main.h"
#include "spi_interface.h"


class STM32SPI : public SPIDevice {
public:
    STM32SPI(SPI_HandleTypeDef* hspi, GPIO_TypeDef* port, uint16_t pin)
        : hspi_(hspi), port_(port), pin_(pin)
    {
        set_delay_function([](uint32_t ms) { HAL_Delay(ms); });
    }

    void init() override {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
    }

    void cs_low() override { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET); }
    void cs_high() override { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET); }

    uint8_t transmit(uint8_t data) override {
        uint8_t rx;
        HAL_SPI_TransmitReceive(hspi_, &data, &rx, 1, HAL_MAX_DELAY);
        return rx;
    }

    void transmit(const uint8_t* data, size_t size) override {
        HAL_SPI_Transmit(hspi_, const_cast<uint8_t*>(data), size, HAL_MAX_DELAY);
    }

    void receive(uint8_t* buffer, size_t size) override {
        HAL_SPI_Receive(hspi_, buffer, size, HAL_MAX_DELAY);
    }

private:
    SPI_HandleTypeDef* hspi_;
    GPIO_TypeDef* port_;
    uint16_t pin_;
};

/*
class STM32_HAL_SPI : public SPIDevice
{
public:
    void init(SPI_HandleTypeDef *spi_port, GPIO_TypeDef *GPIO_CS_Port, uint16_t GPIO_CS_Pin)
    {
    	spi=spi_port;
    	spi_cs_port=GPIO_CS_Port;
    	spi_cs_pin=GPIO_CS_Pin;
    	GPIO_Init();
    	SPI_Init();
    	set_delay_function([](uint32_t ms) { HAL_Delay(ms); });
    };
    void cs_low() override
    {
    	HAL_GPIO_WritePin(spi_cs_port, spi_cs_pin, GPIO_PIN_RESET);
    }
    void cs_high() override
    {
    	HAL_GPIO_WritePin(spi_cs_port, spi_cs_pin, GPIO_PIN_SET);
    }
    uint8_t transmit(uint8_t data) override
    {
    	uint8_t rx;
    	HAL_SPI_TransmitReceive(spi, &data, &rx, 1, HAL_MAX_DELAY);
    	return rx;
    }
    void transmit(const uint8_t* data, size_t size) override
    {
        HAL_SPI_Transmit(spi, data, size, 100);
    }
    void receive(uint8_t* buffer, size_t size) override
    {
    	HAL_SPI_Receive(spi, buffer, size, 100);
    }


private:
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *spi_cs_port;
    uint16_t spi_cs_pin;

    void GPIO_Init()
    {
    	GPIO_InitTypeDef GPIO_InitStruct = {0};
    	GPIO_InitStruct.Pin = spi_cs_pin;
    	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    	GPIO_InitStruct.Pull = GPIO_NOPULL;
    	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    	HAL_GPIO_Init(spi_cs_port, &GPIO_InitStruct);
    }
    void SPI_Init()
	{

	}

    void set_delay_function(DelayFunction func) {
            delay_ms_ = func;
        }
};*/

#endif /* APP_STM32_HAL_SPI_H_ */
