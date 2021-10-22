//
// Created by xhy on 2021/10/21.
//

#ifndef SIMFS_BLOCK_H
#define SIMFS_BLOCK_H

#include "config.h"
#include "utils.h"
#include <stdint.h>
#include <stddef.h>

/**
 *超级块
 */
struct super_block_t {
    uint32_t magic_number;
    uint16_t size_of_sb;
    uint16_t size_of_inode;
    uint16_t size_of_bi;
    uint16_t size_of_block;

    uint32_t total_i_cnt;
    uint32_t total_b_cnt;
    uint32_t free_i_cnt;
    uint32_t free_b_cnt;
    uint32_t disk_size;
};

/**
 * 数据块元数据
 */
struct block_info_t {
    bool occupied;
    uint16_t space_used;
    uint32_t number;
    uint32_t inode_number;
};

/*
 * 数据块
 */
struct data_block_t {
    struct block_info_t info;
    byte_t data[DATA_BLOCK_SIZE];
};

struct dir_entry_t {
    char dir_name[124];
    uint32_t inode_number;
};

typedef struct super_block_t super_block_t;
typedef struct data_block_t data_block_t;
typedef struct block_info_t block_info_t;
typedef struct dir_entry_t dir_entry_t;
#define SUPER_BLOCK_SIZE sizeof(super_block_t)

bool super_block_valid(super_block_t *sb);

void block_init(data_block_t *block);

bool block_dump(data_block_t *block);

bool super_block_dump(super_block_t *super_block);

//向数据块写数据，返回写入的文件长度
int block_write_content(data_block_t *block, const byte_t *data, size_t len);

inode_pointer_t *block_to_inode_poiner_list(data_block_t *block);

bool create_dir_entry(dir_entry_t *dir_entry, const char *file_name, uint32_t inode_index);

#endif //SIMFS_BLOCK_H
