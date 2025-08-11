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

#endif /* APP_STM32_HAL_SPI_H_ */

