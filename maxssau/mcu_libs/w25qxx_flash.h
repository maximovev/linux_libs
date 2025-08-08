/*
 * w25qxx_flash.h
 *
 *  Created on: Aug 8, 2025
 *      Author: maximove
 */

#ifndef APP_W25QXX_FLASH_H_
#define APP_W25QXX_FLASH_H_


#pragma once
#include "flash_interface.h"
#include "w25qxx.h"

class W25QxxFlash : public Flash {
public:
    explicit W25QxxFlash(W25Qxx& driver) : driver_(driver) {}

    ErrorCode init() override {
        auto err = driver_.init();
        return convert_error(err);
    }

    const Info& get_info() const override {
        static Info info;
        const auto& winbond_info = driver_.get_info();

        info.page_size = winbond_info.page_size;
        info.sector_size = winbond_info.sector_size;
        info.block_size = winbond_info.block_size;
        info.total_size = winbond_info.capacity_bytes;
        info.erase_cycles_max = 100000; // 100K циклов для Winbond
        info.supports_suspend = true;

        return info;
    }

    ErrorCode read(uint32_t address, uint8_t* buffer, size_t size) override {
        return convert_error(driver_.read(address, buffer, size));
    }

    ErrorCode write(uint32_t address, const uint8_t* data, size_t size) override {
        return convert_error(driver_.write(address, data, size));
    }

    ErrorCode erase_sector(uint32_t sector) override {
        return convert_error(driver_.erase_sector(sector));
    }

    ErrorCode erase_block(uint32_t block) override {
        return convert_error(driver_.erase_block(block));
    }

    ErrorCode erase_chip() override {
        return convert_error(driver_.erase_chip());
    }

    ErrorCode suspend_erase() override {
        return convert_error(driver_.suspend_erase());
    }

    ErrorCode resume_erase() override {
        return convert_error(driver_.resume_erase());
    }

    bool is_busy() const override {
        return driver_.is_busy();
    }

    ErrorCode enable_write() override {
        return convert_error(driver_.enable_write());
    }

    ErrorCode disable_write() override {
        // Winbond не поддерживает явное отключение записи
        return ErrorCode::Success;
    }

private:
    W25Qxx& driver_;

    ErrorCode convert_error(W25Qxx::ErrorCode err) {
        switch (err) {
            case W25Qxx::ErrorCode::None: return ErrorCode::Success;
            case W25Qxx::ErrorCode::Timeout: return ErrorCode::Timeout;
            case W25Qxx::ErrorCode::InvalidAddress: return ErrorCode::InvalidAddress;
            case W25Qxx::ErrorCode::WriteProtected: return ErrorCode::WriteProtected;
            case W25Qxx::ErrorCode::InvalidParameter: return ErrorCode::NotInitialized;
            case W25Qxx::ErrorCode::VerificationFailed: return ErrorCode::VerificationFailed;
            default: return ErrorCode::WriteError;
        }
    }
};


#endif /* APP_W25QXX_FLASH_H_ */
