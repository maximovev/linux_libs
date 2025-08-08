/*
 * spi_interface.h
 *
 *  Created on: Aug 8, 2025
 *      Author: maximove
 */

#ifndef APP_SPI_INTERFACE_H_
#define APP_SPI_INTERFACE_H_
#pragma once
#include <cstdint>
#include <cstddef>
#include <functional>

class SPIDevice {
public:
    using DelayFunction = std::function<void(uint32_t ms)>;

    virtual ~SPIDevice() = default;

    virtual void init() = 0;
    virtual void cs_low() = 0;
    virtual void cs_high() = 0;
    virtual uint8_t transmit(uint8_t data) = 0;
    virtual void transmit(const uint8_t* data, size_t size) = 0;
    virtual void receive(uint8_t* buffer, size_t size) = 0;

    // Установка функции задержки
    void set_delay_function(DelayFunction func) {
        delay_ms_ = func;
    }

    DelayFunction delay_ms_ = [](uint32_t) {};
protected:
    // Функция задержки по умолчанию (пустая)

};


#endif /* APP_SPI_INTERFACE_H_ */
