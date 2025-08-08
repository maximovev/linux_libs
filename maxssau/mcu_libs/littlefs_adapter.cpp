/*
 * littlefs_adapter.cpp
 *
 *  Created on: Aug 8, 2025
 *      Author: maximove
 */
#include "littlefs_adapter.h"
#include <cstring>

// Обработчики для LittleFS
int LittleFSAdapter::static_read(const struct lfs_config* c, lfs_block_t block,
                                lfs_off_t off, void* buffer, lfs_size_t size) {
    LittleFSAdapter* adapter = reinterpret_cast<LittleFSAdapter*>(c->context);
    if (!adapter->flash_) return LFS_ERR_IO;

    uint32_t address = block * c->block_size + off;
    return adapter->flash_->read(address, static_cast<uint8_t*>(buffer), size) ==
           Flash::ErrorCode::Success ? 0 : LFS_ERR_IO;
}

int LittleFSAdapter::static_prog(const struct lfs_config* c, lfs_block_t block,
                                lfs_off_t off, const void* buffer, lfs_size_t size) {
    LittleFSAdapter* adapter = reinterpret_cast<LittleFSAdapter*>(c->context);
    if (!adapter->flash_) return LFS_ERR_IO;

    uint32_t address = block * c->block_size + off;
    return adapter->flash_->write(address, static_cast<const uint8_t*>(buffer), size) ==
           Flash::ErrorCode::Success ? 0 : LFS_ERR_IO;
}

int LittleFSAdapter::static_erase(const struct lfs_config* c, lfs_block_t block) {
    LittleFSAdapter* adapter = reinterpret_cast<LittleFSAdapter*>(c->context);
    if (!adapter->flash_) return LFS_ERR_IO;

    return adapter->flash_->erase_block(block) ==
           Flash::ErrorCode::Success ? 0 : LFS_ERR_IO;
}

int LittleFSAdapter::static_sync(const struct lfs_config* c) {
    return 0;
}

LittleFSAdapter::LittleFSAdapter() {
    memset(&cfg_, 0, sizeof(cfg_));
    cfg_.context = this;
    cfg_.read = static_read;
    cfg_.prog = static_prog;
    cfg_.erase = static_erase;
    cfg_.sync = static_sync;
}

LittleFSAdapter::~LittleFSAdapter() {
    unmount();
}

bool LittleFSAdapter::init(Flash& flash) {
    if (is_initialized()) {
        unmount();
    }

    flash_ = &flash;
    const auto& info = flash_->get_info();

    if (info.block_size == 0 || info.total_size == 0) {
        flash_ = nullptr;
        return false;
    }

    cfg_.read_size = 1;
    cfg_.prog_size = info.page_size;
    cfg_.block_size = info.block_size;
    cfg_.block_count = info.total_size / info.block_size;

    if (cfg_.block_count == 0) {
        flash_ = nullptr;
        return false;
    }

    cfg_.block_cycles = 500;
    cfg_.cache_size = 256;
    cfg_.lookahead_size = 16;

    return true;
}

bool LittleFSAdapter::mount() {
    if (!is_initialized() || mounted_) return false;

    int err = lfs_mount(&lfs_, &cfg_);
    if (err != LFS_ERR_OK) {
        err = lfs_format(&lfs_, &cfg_);
        if (err != LFS_ERR_OK) return false;

        err = lfs_mount(&lfs_, &cfg_);
        if (err != LFS_ERR_OK) return false;
    }

    mounted_ = true;
    return true;
}

bool LittleFSAdapter::unmount() {
    if (!mounted_) return true;

    int err = lfs_unmount(&lfs_);
    if (err != LFS_ERR_OK) return false;

    mounted_ = false;
    return true;
}

bool LittleFSAdapter::format() {
    if (!is_initialized()) return false;
    if (mounted_) unmount();

    int err = lfs_format(&lfs_, &cfg_);
    return err == LFS_ERR_OK;
}

// Файловые операции
int LittleFSAdapter::open(const char* path, int flags) {
    if (!mounted_) return -1;

    lfs_file_t* file = new lfs_file_t();
    int err = lfs_file_open(&lfs_, file, path, flags);
    if (err != LFS_ERR_OK) {
        delete file;
        return -1;
    }
    return reinterpret_cast<int>(file);
}

int LittleFSAdapter::close(int fd) {
    if (!mounted_) return -1;

    lfs_file_t* file = reinterpret_cast<lfs_file_t*>(fd);
    int err = lfs_file_close(&lfs_, file);
    delete file;
    return err == LFS_ERR_OK ? 0 : -1;
}

lfs_ssize_t LittleFSAdapter::read(int fd, void* buf, lfs_size_t count) {
    if (!mounted_) return -1;

    lfs_file_t* file = reinterpret_cast<lfs_file_t*>(fd);
    return lfs_file_read(&lfs_, file, buf, count);
}

lfs_ssize_t LittleFSAdapter::write(int fd, const void* buf, lfs_size_t count) {
    if (!mounted_) return -1;

    lfs_file_t* file = reinterpret_cast<lfs_file_t*>(fd);
    return lfs_file_write(&lfs_, file, buf, count);
}

lfs_soff_t LittleFSAdapter::seek(int fd, lfs_soff_t offset, int whence) {
    if (!mounted_) return -1;

    lfs_file_t* file = reinterpret_cast<lfs_file_t*>(fd);
    return lfs_file_seek(&lfs_, file, offset, whence);
}

int LittleFSAdapter::stat(const char* path, struct lfs_info* info) {
    if (!mounted_) return -1;
    return lfs_stat(&lfs_, path, info);
}

int LittleFSAdapter::remove(const char* path) {
    if (!mounted_) return -1;
    return lfs_remove(&lfs_, path);
}

int LittleFSAdapter::rename(const char* oldpath, const char* newpath) {
    if (!mounted_) return -1;
    return lfs_rename(&lfs_, oldpath, newpath);
}

// Операции с директориями
int LittleFSAdapter::mkdir(const char* path) {
    if (!mounted_) return -1;
    return lfs_mkdir(&lfs_, path);
}

int LittleFSAdapter::dir_open(const char* path) {
    if (!mounted_) return -1;

    lfs_dir_t* dir = new lfs_dir_t();
    int err = lfs_dir_open(&lfs_, dir, path);
    if (err != LFS_ERR_OK) {
        delete dir;
        return -1;
    }
    return reinterpret_cast<int>(dir);
}

int LittleFSAdapter::dir_read(int dir, struct lfs_info* info) {
    if (!mounted_) return -1;

    lfs_dir_t* dirp = reinterpret_cast<lfs_dir_t*>(dir);
    return lfs_dir_read(&lfs_, dirp, info);
}

int LittleFSAdapter::dir_close(int dir) {
    if (!mounted_) return -1;

    lfs_dir_t* dirp = reinterpret_cast<lfs_dir_t*>(dir);
    int err = lfs_dir_close(&lfs_, dirp);
    delete dirp;
    return err == LFS_ERR_OK ? 0 : -1;
}

int LittleFSAdapter::dir_seek(int dir, lfs_soff_t offset) {
    if (!mounted_) return -1;

    lfs_dir_t* dirp = reinterpret_cast<lfs_dir_t*>(dir);
    return lfs_dir_seek(&lfs_, dirp, offset);
}

lfs_soff_t LittleFSAdapter::dir_tell(int dir) {
    if (!mounted_) return -1;

    lfs_dir_t* dirp = reinterpret_cast<lfs_dir_t*>(dir);
    return lfs_dir_tell(&lfs_, dirp);
}

int LittleFSAdapter::dir_rewind(int dir) {
    if (!mounted_) return -1;

    lfs_dir_t* dirp = reinterpret_cast<lfs_dir_t*>(dir);
    return lfs_dir_rewind(&lfs_, dirp);
}

// Информация о файловой системе
lfs_ssize_t LittleFSAdapter::fs_size() {
    if (!mounted_) return -1;
    return lfs_fs_size(&lfs_);
}

int LittleFSAdapter::fs_traverse(int (*cb)(void*, lfs_block_t), void* data) {
    if (!mounted_) return -1;
    return lfs_fs_traverse(&lfs_, cb, data);
}
