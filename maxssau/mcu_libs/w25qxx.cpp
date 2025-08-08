/*
 * w25qxx.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: maximove
 */


#include "w25qxx.h"
#include <algorithm>
#include <math.h>

W25Qxx::W25Qxx(SPIDevice& spi) : spi_(spi) {}

W25Qxx::ErrorCode W25Qxx::init() {
    spi_.init();

    if (auto err = read_jedec_id(); err != ErrorCode::None)
        return err;

    calculate_capacity();
    initialized_ = true;
    return ErrorCode::None;
}

W25Qxx::ErrorCode W25Qxx::read_jedec_id() {
    spi_.cs_low();
    spi_.transmit(READ_JEDEC_ID);

    uint8_t buf[3];
    spi_.receive(buf, sizeof(buf));

    spi_.cs_high();

    info_.manufacturer_id = buf[0];
    info_.device_id = (buf[1] << 8) | buf[2];

    return ErrorCode::None;
}

void W25Qxx::calculate_capacity() {
    // Определение объема по JEDEC ID
    switch (info_.device_id & 0xFF) {
        case 0x18: info_.capacity_bytes = 32 * 1024 * 1024; break; // W25Q256
        case 0x17: info_.capacity_bytes = 16 * 1024 * 1024; break; // W25Q128
        case 0x16: info_.capacity_bytes =  8 * 1024 * 1024; break; // W25Q64
        case 0x15: info_.capacity_bytes =  4 * 1024 * 1024; break; // W25Q32
        case 0x14: info_.capacity_bytes =  2 * 1024 * 1024; break; // W25Q16
        case 0x13: info_.capacity_bytes =  1 * 1024 * 1024; break; // W25Q80
        default:   info_.capacity_bytes =  4 * 1024 * 1024; break; // Default 4MB
    }

    // Стандартные параметры
    info_.page_size = 256;
    info_.page_count = info_.capacity_bytes / info_.page_size;
    info_.sector_size = 4096;
    info_.sector_count = info_.capacity_bytes / info_.sector_size;
    info_.block_size = 65536;
    info_.block_count = info_.capacity_bytes / info_.block_size;
}

W25Qxx::ErrorCode W25Qxx::wait_ready(uint32_t timeout_ms) const {
    while (is_busy()) {
        spi_.delay_ms_(1);
        if (timeout_ms-- == 0)
            return ErrorCode::Timeout;
    }
    return ErrorCode::None;
}

W25Qxx::ErrorCode W25Qxx::enable_write() {
    if (is_busy())
        return ErrorCode::Timeout;

    spi_.cs_low();
    spi_.transmit(WRITE_ENABLE);
    spi_.cs_high();

    return is_write_enabled() ? ErrorCode::None : ErrorCode::WriteProtected;
}

W25Qxx::ErrorCode W25Qxx::read(uint32_t address, uint8_t* buffer, size_t size) const {
    if (!initialized_) return ErrorCode::InvalidParameter;
    if (address + size > info_.capacity_bytes) return ErrorCode::InvalidAddress;

    if (auto err = wait_ready(); err != ErrorCode::None)
        return err;

    spi_.cs_low();
    spi_.transmit(FAST_READ);
    spi_.transmit(static_cast<uint8_t>(address >> 16));
    spi_.transmit(static_cast<uint8_t>(address >> 8));
    spi_.transmit(static_cast<uint8_t>(address));
    spi_.transmit(0x00); // Dummy byte

    for (size_t i = 0; i < size; ++i) {
        buffer[i] = spi_.transmit(0x00);
    }
    spi_.cs_high();

    return ErrorCode::None;
}

W25Qxx::ErrorCode W25Qxx::execute_write(uint32_t address, const uint8_t* data, size_t size) {
    size_t written = 0;

    while (written < size) {
        uint32_t page = address / info_.page_size;
        uint32_t offset = address % info_.page_size;
        size_t chunk = 0;

        if((size - written)<= (info_.page_size - offset))
        {
        	chunk=size - written;
        }
        else
        {
        	chunk=info_.page_size - offset;
        }

        if (auto err = enable_write(); err != ErrorCode::None)
            return err;

        spi_.cs_low();
        spi_.transmit(PAGE_PROGRAM);
        spi_.transmit(static_cast<uint8_t>(address >> 16));
        spi_.transmit(static_cast<uint8_t>(address >> 8));
        spi_.transmit(static_cast<uint8_t>(address));

        spi_.transmit(data + written, chunk);
        spi_.cs_high();

        if (auto err = wait_ready(); err != ErrorCode::None)
            return err;

        // Верификация записи (опционально)
        // if (!verify_write(address, data + written, chunk))
        //     return ErrorCode::VerificationFailed;

        written += chunk;
        address += chunk;
    }

    return ErrorCode::None;
}

W25Qxx::ErrorCode W25Qxx::write(uint32_t address, const uint8_t* data, size_t size) {
    if (!initialized_) return ErrorCode::InvalidParameter;
    if (address + size > info_.capacity_bytes) return ErrorCode::InvalidAddress;
    if (size == 0) return ErrorCode::None;

    // Проверка на запись в пределах одного сектора
    const uint32_t start_sector = address / info_.sector_size;
    const uint32_t end_sector = (address + size - 1) / info_.sector_size;

    if (start_sector != end_sector) {
        // Для кросс-секторной записи требуется специальная обработка
        return ErrorCode::InvalidAddress;
    }

    return execute_write(address, data, size);
}

W25Qxx::ErrorCode W25Qxx::erase_sector(uint32_t sector) {
    if (!initialized_) return ErrorCode::InvalidParameter;
    if (sector >= info_.sector_count) return ErrorCode::InvalidAddress;

    if (auto err = enable_write(); err != ErrorCode::None)
        return err;

    const uint32_t address = sector * info_.sector_size;

    spi_.cs_low();
    spi_.transmit(SECTOR_ERASE_4K);
    spi_.transmit(static_cast<uint8_t>(address >> 16));
    spi_.transmit(static_cast<uint8_t>(address >> 8));
    spi_.transmit(static_cast<uint8_t>(address));
    spi_.cs_high();

    return wait_ready(500);
}

W25Qxx::ErrorCode W25Qxx::erase_block(uint32_t block) {
    if (!initialized_) return ErrorCode::InvalidParameter;
    if (block >= info_.block_count) return ErrorCode::InvalidAddress;

    if (auto err = enable_write(); err != ErrorCode::None)
        return err;

    const uint32_t address = block * info_.block_size;

    spi_.cs_low();
    spi_.transmit(BLOCK_ERASE_64K);
    spi_.transmit(static_cast<uint8_t>(address >> 16));
    spi_.transmit(static_cast<uint8_t>(address >> 8));
    spi_.transmit(static_cast<uint8_t>(address));
    spi_.cs_high();

    return wait_ready(2000);
}

W25Qxx::ErrorCode W25Qxx::erase_chip() {
    if (!initialized_) return ErrorCode::InvalidParameter;

    if (auto err = enable_write(); err != ErrorCode::None)
        return err;

    spi_.cs_low();
    spi_.transmit(CHIP_ERASE);
    spi_.cs_high();

    return wait_ready(20000); // До 20 секунд для полного стирания
}

uint8_t W25Qxx::read_status_register(uint8_t reg_num) const {
    switch (reg_num) {
        case 1: return read_status_register_impl(READ_STATUS_REG1);
        case 2: return read_status_register_impl(READ_STATUS_REG2);
        case 3: return read_status_register_impl(READ_STATUS_REG3);
        default: return 0;
    }
}

// Внутренняя реализация чтения статуса (константная)
uint8_t W25Qxx::read_status_register_impl(uint8_t cmd) const {
    spi_.cs_low();
    spi_.transmit(cmd);
    uint8_t status = spi_.transmit(0x00);
    spi_.cs_high();
    return status;
}

W25Qxx::ErrorCode W25Qxx::write_status_register(uint8_t reg_num, uint8_t value) {
    if (auto err = enable_write(); err != ErrorCode::None)
        return err;

    spi_.cs_low();
    spi_.transmit(WRITE_STATUS_REG);
    if (reg_num >= 1 && reg_num <= 3) {
        // Для мульти-байтовых регистров
        uint8_t data[3] = {0};
        data[reg_num - 1] = value;
        spi_.transmit(data, 3);
    } else {
        spi_.transmit(value);
    }
    spi_.cs_high();

    return wait_ready();
}

W25Qxx::ErrorCode W25Qxx::suspend_erase() {
    spi_.cs_low();
    spi_.transmit(ERASE_SUSPEND);
    spi_.cs_high();
    return wait_ready();
}

W25Qxx::ErrorCode W25Qxx::resume_erase() {
    spi_.cs_low();
    spi_.transmit(ERASE_RESUME);
    spi_.cs_high();
    return wait_ready();
}

W25Qxx::ErrorCode W25Qxx::power_down() const {
    spi_.cs_low();
    spi_.transmit(POWER_DOWN);
    spi_.cs_high();
    spi_.delay_ms_(1);
    return ErrorCode::None;
}

W25Qxx::ErrorCode W25Qxx::release_power_down() {
    spi_.cs_low();
    spi_.transmit(RELEASE_POWER_DOWN);
    spi_.transmit(0x00); // Dummy
    spi_.cs_high();
    spi_.delay_ms_(1); // Tres1 = 3μs typical
    return ErrorCode::None;
}
