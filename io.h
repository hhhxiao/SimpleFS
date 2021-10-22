//
// Created by xhy on 2021/10/21.
//

#ifndef SIMFS_IO_H
#define SIMFS_IO_H

#include "disk.h"
#include "inode.h"
#include "block.h"

//根据index从inode获取第index个s块id
uint32_t inode_get_block(inode_t *inode, uint32_t index, disk_t *disk);

inode_pointer_t *inode_get_pointer(inode_t *inode, uint32_t index, disk_t *disk);

uint32_t inode_add_new_block(inode_t *inode, disk_t *disk);


#endif //SIMFS_IO_H
