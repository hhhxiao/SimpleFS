//
// Created by xhy on 2021/10/21.
//

#ifndef SIMFS_INODE_H
#define SIMFS_INODE_H

#include <stdint.h>
#include "utils.h"
#include "config.h"
#include <time.h>


enum FILETYPE {
    None = -1,
    Normal = 0,
    Dir = 1,
    Charctor = 2,
    Block = 3,
    Socket = 4,
    Pipe = 5,
    Link = 6
};

struct file_mode_t {
    enum FILETYPE file_type;
    byte_t up: 3;
    byte_t gp: 3;
    byte_t ap: 3;
};

struct inode_t {
    bool occupied;
    uint32_t number;
    uint32_t file_size;
    struct file_mode_t mode;
    int8_t uid;
    int8_t gid;
    time_t c_time;
    time_t m_time;
    time_t a_time;
    uint8_t links;
    inode_pointer_t direct[INODE_DIRECT_NUM];
    uint32_t block_used;
    uint16_t dir_cnt;
    inode_pointer_t singly;
    inode_pointer_t doubly;
    inode_pointer_t triple;
};


typedef struct file_mode_t file_mode_t;
typedef struct inode_t inode_t;

#define INODE_SIZE sizeof(inode_t)

void file_mode_init(file_mode_t *file_mode);

void inode_init(inode_t *inode);

uint32_t inode_fetch_block_id(inode_t *inode, int index);

bool inode_dump(inode_t *inode);

#endif //SIMFS_INODE_H
