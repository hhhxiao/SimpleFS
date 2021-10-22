#ifndef SIMFS_DISK_H
#define SIMFS_DISK_H

#include <stdint.h>
#include "utils.h"
#include "block.h"
#include "inode.h"

struct disk_t {
    super_block_t sb;
    inode_t *inode_list;
    struct block_info_t *data_block_bitmap;
    data_block_t *data_block_list;
};


typedef struct disk_t disk_t;


bool disk_init(disk_t *disk, uint32_t disk_size);

void disk_free(disk_t *disk);

bool disk_visual(disk_t *disk);

//找一个未被使用的inode
int disk_find_free_inode(disk_t *disk);

//找一个未被使用的block
int disk_find_free_data_block(disk_t *disk);

int disk_assign_inode(disk_t *disk);

inode_t *disk_fetch_inode(disk_t *disk, int index);

data_block_t *disk_fetch_block(disk_t *disk, int index);


bool disk_save(disk_t *disk, const char *file_name);

bool disk_load(disk_t *disk, const char *file_name);

#endif

