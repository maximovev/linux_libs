/*
 * flash_interface.h
 *
 *  Created on: Aug 8, 2025
 *      Author: maximove
 */

#ifndef APP_FLASH_INTERFACE_H_
#define APP_FLASH_INTERFACE_H_

#pragma once
#include <cstdint>
#include <cstddef>

class Flash {
public:
    enum class ErrorCode {
        Success,
        ReadError,
        WriteError,
        EraseError,
        Timeout,
        InvalidAddress,
        NotInitialized,
        VerificationFailed,
        WriteProtected
    };

    struct Info {
        uint32_t page_size;
        uint32_t sector_size;
        uint32_t block_size;
        uint32_t total_size;
        uint32_t erase_cycles_max;
        bool supports_suspend;
    };

    virtual ~Flash() = default;

    virtual ErrorCode init() = 0;
    virtual const Info& get_info() const = 0;

    virtual ErrorCode read(uint32_t address, uint8_t* buffer, size_t size) = 0;
    virtual ErrorCode write(uint32_t address, const uint8_t* data, size_t size) = 0;

    virtual ErrorCode erase_sector(uint32_t sector) = 0;
    virtual ErrorCode erase_block(uint32_t block) = 0;
    virtual ErrorCode erase_chip() = 0;

    virtual ErrorCode suspend_erase() = 0;
    virtual ErrorCode resume_erase() = 0;

    virtual bool is_busy() const = 0;
    virtual ErrorCode enable_write() = 0;
    virtual ErrorCode disable_write() = 0;
};



#endif /* APP_FLASH_INTERFACE_H_ */
