/*
 * w25qxx.h
 *
 *  Created on: Aug 8, 2025
 *      Author: maximove
 */

#ifndef APP_W25QXX_H_
#define APP_W25QXX_H_

#pragma once
#include "spi_interface.h"
#include <cstdint>

class W25Qxx {
public:
    enum class ErrorCode {
        None,
        Timeout,
        InvalidAddress,
        WriteProtected,
        InvalidParameter,
        VerificationFailed
    };

    struct Info {
        uint16_t manufacturer_id;
        uint16_t device_id;
        uint32_t capacity_bytes;
        uint16_t page_size;
        uint32_t page_count;
        uint32_t sector_size;
        uint32_t sector_count;
        uint32_t block_size;
        uint32_t block_count;
    };

    explicit W25Qxx(SPIDevice& spi);

    ErrorCode init();
    ErrorCode get_id();
    const Info& get_info() const { return info_; }

    ErrorCode read(uint32_t address, uint8_t* buffer, size_t size) const;
    ErrorCode write(uint32_t address, const uint8_t* data, size_t size);
    ErrorCode erase_sector(uint32_t sector);
    ErrorCode erase_block(uint32_t block);
    ErrorCode erase_chip();

    ErrorCode suspend_erase();
    ErrorCode resume_erase();

    ErrorCode power_down() const;
    ErrorCode release_power_down() const;

    // Константные методы для чтения статуса
    uint8_t read_status_register(uint8_t reg_num = 1) const;
    bool is_busy() const { return (read_status_register() & 0x01) != 0; }
    bool is_write_enabled() const { return (read_status_register() & 0x02) != 0; }

    ErrorCode enable_write();

private:
    SPIDevice& spi_;
    Info info_{};
    bool initialized_ = false;

    // Command set
    enum Commands : uint8_t {
        WRITE_ENABLE     = 0x06,
        WRITE_DISABLE    = 0x04,
        READ_STATUS_REG1 = 0x05,
        READ_STATUS_REG2 = 0x35,
        READ_STATUS_REG3 = 0x15,
        WRITE_STATUS_REG = 0x01,
        PAGE_PROGRAM     = 0x02,
        BLOCK_ERASE_64K  = 0xD8,
        SECTOR_ERASE_4K  = 0x20,
        CHIP_ERASE       = 0xC7,
        POWER_DOWN       = 0xB9,
        RELEASE_POWER_DOWN = 0xAB,
        READ_DATA        = 0x03,
        FAST_READ        = 0x0B,
        READ_JEDEC_ID    = 0x9F,
        ERASE_SUSPEND    = 0x75,
        ERASE_RESUME     = 0x7A
    };

    ErrorCode wait_ready(uint32_t timeout_ms = 3000) const;
    ErrorCode read_jedec_id();
    void calculate_capacity();

    ErrorCode execute_write(uint32_t address, const uint8_t* data, size_t size);
    uint8_t read_status_register_impl(uint8_t cmd) const;
    ErrorCode release_power_down();
    ErrorCode write_status_register(uint8_t reg_num, uint8_t value);
};


#endif /* APP_W25QXX_H_ */
