//
// Created by xhy on 2021/10/21.
//

#include "block.h"
#include "inode.h"
#include "utils.h"


bool super_block_valid(super_block_t *sb) {
    if (sb == NULL)
        return false;
    if (sb->size_of_block != DATA_BLOCK_SIZE)
        return false;
    if (sb->size_of_inode != sizeof(inode_t))
        return false;
    if (sb->size_of_sb != sizeof(super_block_t))
        return false;
    if (sb->size_of_bi != sizeof(block_info_t))
        return false;
    uint32_t total_size = sb->size_of_sb + sb->size_of_inode * sb->total_i_cnt
                          + (sb->size_of_block + sb->size_of_bi) * sb->total_b_cnt;
    if (total_size != sb->disk_size) {
        return false;
    }
    return true;
}


void block_init(data_block_t *block) {
    block->info.occupied = false;
    block->info.number = -1;
    block->info.inode_number = -1;
    block->info.space_used = 0;
    for (int i = 0; i < DATA_BLOCK_SIZE; i++) {
        block->data[i] = 0;
    }
}


inode_pointer_t *block_to_inode_poiner_list(data_block_t *block) {
    if (block == NULL) {
        return NULL;
    }
    void *data = block->data;
    return (inode_pointer_t *) data;
}

bool block_dump(data_block_t *block) {
    if (block == NULL) {
        ERR("got a nullptr of data_block_t");
        return false;
    }
    printf("Data block: %d    Inode: %d Space: %d\n", block->info.number, block->info.inode_number,
           block->info.space_used);
    for (int i = 0; i < min(block->info.space_used, 128u); i++) {
        printf("%c", block->data[i]);
    }
    printf("\n");
    return true;
}

bool super_block_dump(super_block_t *super_block) {
    if (super_block == NULL) {
        ERR("got a nullptr of super_block_t");
        return false;
    }
    INFO("SIZE:     super block: %dB    inode: %dB    data block info: %dB    data block: %dB\n",
         super_block->size_of_sb,
         super_block->size_of_inode,
         super_block->size_of_bi,
         super_block->size_of_block
    );
    INFO("INFO:     Inodes: %d/%d    Data block: %d/%d\n",
         super_block->free_i_cnt, super_block->total_i_cnt,
         super_block->free_b_cnt, super_block->total_b_cnt
    );
    INFO("Disk Size: %d\n", super_block->disk_size);

    if (super_block_valid(super_block) == false) {
        ERR("super block is invalid\n");
        return false;
    }
    return true;
}

int block_write_content(data_block_t *block, const byte_t *data, size_t len) {
    int res = (int) min(len, DATA_BLOCK_SIZE - block->info.space_used);
    int idx_b = block->info.space_used;
    int idx_d = 0;
    for (int i = 0; i < res; i++) {
        block->data[idx_b] = data[idx_d];
        ++idx_b;
        ++idx_d;
    }
    block->info.space_used += res;
    return res;
}

bool dir_entry_init(dir_entry_t *dir_entry, const char *file_name, uint32_t inode_index) {
    memset(dir_entry, 0, sizeof(dir_entry_t));
    size_t len = strlen(file_name);
    if (len >= 124) {
        return false;
    }
    for (int i = 0; i < len; i++) {
        dir_entry->dir_name[i] = file_name[i];
        dir_entry->dir_name[123] = 0;
    }
    dir_entry->inode_number = inode_index;
    return true;
}
