/*
 * littlefs_adapter.h
 *
 *  Created on: Aug 8, 2025
 *      Author: maximove
 */

#ifndef APP_LITTLEFS_ADAPTER_H_
#define APP_LITTLEFS_ADAPTER_H_


#pragma once
#include "flash_interface.h"
#include "lfs.h"
#include <cstddef> // Для size_t

class LittleFSAdapter {
public:
    LittleFSAdapter();  // Пустой конструктор
    ~LittleFSAdapter();

    bool init(Flash& flash);  // Инициализация флеш-памяти

    bool mount();
    bool unmount();
    bool format();

    // Файловые операции
    int open(const char* path, int flags);
    int close(int fd);
    lfs_ssize_t read(int fd, void* buf, lfs_size_t count);
    lfs_ssize_t write(int fd, const void* buf, lfs_size_t count);
    lfs_soff_t seek(int fd, lfs_soff_t offset, int whence);
    int stat(const char* path, struct lfs_info* info);
    int remove(const char* path);
    int rename(const char* oldpath, const char* newpath);

    // Директории
    int mkdir(const char* path);
    int dir_open(const char* path);
    int dir_read(int dir, struct lfs_info* info);
    int dir_close(int dir);
    int dir_seek(int dir, lfs_soff_t offset);
    lfs_soff_t dir_tell(int dir);
    int dir_rewind(int dir);

    // Информация о файловой системе
    lfs_ssize_t fs_size();
    int fs_traverse(int (*cb)(void*, lfs_block_t), void* data);

    // Проверка инициализации
    bool is_initialized() const { return flash_ != nullptr; }
    bool is_mounted() const { return mounted_; }

private:
    Flash* flash_ = nullptr;
    lfs_t lfs_;
    lfs_config cfg_;
    bool mounted_ = false;

    // Статические функции-обработчики для LittleFS
    static int static_read(const struct lfs_config* c, lfs_block_t block,
                          lfs_off_t off, void* buffer, lfs_size_t size);

    static int static_prog(const struct lfs_config* c, lfs_block_t block,
                          lfs_off_t off, const void* buffer, lfs_size_t size);

    static int static_erase(const struct lfs_config* c, lfs_block_t block);
    static int static_sync(const struct lfs_config* c);

    // Вспомогательные функции
    bool init_config();
};

#endif /* APP_LITTLEFS_ADAPTER_H_ */
