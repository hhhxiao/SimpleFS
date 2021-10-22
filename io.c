//
// Created by xhy on 2021/10/21.
//

#include "io.h"
#include <stdio.h>


inode_pointer_t *inode_get_pointer(inode_t *inode, uint32_t index, disk_t *disk) {
    if (inode == NULL || disk == NULL || index < 0) {
        return NULL;
    }
    if (index >= MAX_BLOCK_DIRECT + MAX_BLOCK_SIGNY + MAX_BLOCK_DOUBLY) {
        return NULL;
    }
    int max_used_block = MAX_BLOCK_SIGNY;
    if (index < max_used_block) {
        return inode->direct + index;
    }

    max_used_block += MAX_BLOCK_SIGNY;
    if (index < max_used_block) {
        index -= MAX_BLOCK_DIRECT;
        data_block_t *block = disk_fetch_block(disk, (int) inode->singly);
        if (block == NULL) {
            fprintf(stderr, "unknown error occured");
            return NULL;
        }
        inode_pointer_t *inode_table = block_to_inode_poiner_list(block);
        return inode_table + index;
    }
    s
    max_used_block += MAX_BLOCK_DIRECT;
    if (index < max_used_block) {
        index -= (MAX_BLOCK_DIRECT + MAX_BLOCK_SIGNY);
        uint32_t l1_idx = index / INODE_POINTER_SIZE;
        uint32_t l2_idx = index % INODE_POINTER_SIZE;

        //inode直接指向的block
        data_block_t *l1_block = disk_fetch_block(disk, (int) inode->doubly);
        if (l1_block == NULL) {
            fprintf(stderr, "unknown error occured");
            return NULL;
        }

        //从inode直接指向的block中索引出二级block
        inode_pointer_t *inode_table_1 = block_to_inode_poiner_list(l1_block);
        //二级block
        data_block_t *l2_block = disk_fetch_block(disk, (int) inode_table_1[l1_idx]);
        inode_pointer_t *inode_table_2 = block_to_inode_poiner_list(l2_block);
        return inode_table_2 + l2_idx;
    }

    return NULL;
}

uint32_t inode_get_block(inode_t *inode, uint32_t index, disk_t *disk) {
    inode_pointer_t *p = inode_get_pointer(inode, index, disk);
    if (p == NULL) {
        return -1;
    } else {
        return *p;
    }
}
s
uint32_t inode_add_new_block(inode_t *inode, disk_t *disk) {
    if (inode == NULL) {
        return -1;
    }

    inode_pointer_t *p = inode_get_pointer(inode, inode->block_used + 1, disk);
    if (p == NULL) {
        return -1;
    }

    uint32_t block_id = disk_find_free_data_block(disk);
    if (block_id == -1) {
        fprintf(stderr, "disk doesn't have enough memory");
    }
    data_block_t *block = disk_fetch_block(disk, (int) block_id);
    block->info.occupied = 1;
    block->info.inode_number = inode->number;
    *p = block_id;
    inode->block_used++;
    disk->sb.free_b_cnt--;
    return block_id;
}
