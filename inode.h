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
    None = '?',
    Normal = '-',
    Dir = 'd',
    Charctor = 'c',
    Block = 'b',
    Socket = 's',
    Pipe = 'p',
    Link = 'l'
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
    uint16_t cnt_file_num;
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
