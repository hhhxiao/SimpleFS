//
// Created by xhy on 2021/10/21.
//
#include <stdint.h>

#ifndef SIMFS_CONFIG_H
#define SIMFS_CONFIG_H

#define  DATA_BLOCK_SIZE 1024
typedef uint32_t inode_pointer_t;

#define INODE_POINTER_SIZE sizeof(inode_pointer_t)
//128 = 1204 / 8
#define INODE_PTR_NUM_IN_BLOCK ((DATA_BLOCK_SIZE)/(INODE_POINTER_SIZE))

#define INODE_DIRECT_NUM 12
#define MAX_BLOCK_DIRECT 12
#define MAX_BLOCK_SIGNY INODE_PTR_NUM_IN_BLOCK
#define MAX_BLOCK_DOUBLY ((INODE_PTR_NUM_IN_BLOCK)*(INODE_PTR_NUM_IN_BLOCK))
#define MAX_BLOCK_TRIPLE ((INODE_PTR_NUM_IN_BLOCK)*(INODE_PTR_NUM_IN_BLOCK)*(INODE_PTR_NUM_IN_BLOCK))

#define INODE_SPACE_RATIO 0.01
#define MAGIC_NUMBER 0x1234

#endif //SIMFS_CONFIG_H
