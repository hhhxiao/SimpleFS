#include "disk.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include "block.h"
#include "inode.h"
#include <assert.h>


static void init_super_block(super_block_t *super_block) {
    if (super_block == NULL) {
        return;
    }

    super_block->magic_number = MAGIC_NUMBER;
    super_block->size_of_block = DATA_BLOCK_SIZE;
    super_block->size_of_inode = sizeof(inode_t);
    super_block->size_of_sb = sizeof(super_block_t);
    super_block->size_of_bi = sizeof(block_info_t);
}

inode_t *disk_fetch_inode(disk_t *disk, int index) {
    if (index < 0 || index >= disk->sb.total_i_cnt) {
        return NULL;
    } else {
        return disk->inode_list + index;
    }
}

data_block_t *disk_fetch_block(disk_t *disk, int index) {
    if (index < 0 || index >= disk->sb.total_b_cnt) {
        return NULL;
    } else {
        return disk->data_block_list + index;
    }
}

static void disk_reset_inodes(disk_t *disk) {
    for (int i = 0; i < disk->sb.total_i_cnt; i++) {
        inode_init(&disk->inode_list[i]);
        disk->inode_list[i].number = i;
    }
}

static void disk_reset_blocks(disk_t *disk) {
    for (int i = 0; i < disk->sb.total_b_cnt; i++) {
        block_init(&disk->data_block_list[i]);
        disk->data_block_list[i].info.number = i;
    }
}


int disk_init(disk_t *disk, uint32_t disk_size) {
    if (disk_size <= SUPER_BLOCK_SIZE) {
        LOG("given size is too small(%d < %d)", disk_size, SUPER_BLOCK_SIZE);
        return false;
    }

    assert(sizeof(struct dir_entry_t) == 128);
    init_super_block(&disk->sb);

    super_block_t *super_block = &disk->sb;

    //total - super block
    uint32_t free_space = disk_size - SUPER_BLOCK_SIZE;
    LOG("%d", free_space);
    //
    uint32_t estimated_inode_space = (uint32_t) (free_space * INODE_SPACE_RATIO);
    LOG("%d", estimated_inode_space);

    uint32_t total_inode_count = estimated_inode_space / sizeof(inode_t);
    disk->sb.total_i_cnt = total_inode_count;
    disk->sb.free_i_cnt = total_inode_count;

    free_space -= total_inode_count * sizeof(inode_t);
    LOG("%u", free_space);

    //计算数据块的数量(总空间/元数据大小+数据块大小)
    uint32_t block_count = free_space / (sizeof(block_info_t) + DATA_BLOCK_SIZE);
    disk->sb.total_b_cnt = block_count;
    disk->sb.free_b_cnt = block_count;

    //计算实际的磁盘大小
    uint32_t actual_disk_size =
            SUPER_BLOCK_SIZE + total_inode_count * INODE_SIZE + block_count * (DATA_BLOCK_SIZE + sizeof(block_info_t));
    disk->sb.disk_size = actual_disk_size;

    //给inode分配内存
    disk->inode_list = (inode_t *) malloc(sizeof(inode_t) * total_inode_count);

    if (disk->inode_list == NULL) {
        ERR("memory alloc failure");
        free(disk);
        return false;
    }

    //给block分配内存
    disk->data_block_list = (data_block_t *) malloc(sizeof(data_block_t) * block_count);
    if (disk->data_block_list == NULL) {
        free(disk->inode_list);
        free(disk);
        ERR("memory alloc failure");
        return false;
    }
//    LOG("actual disk size is %d(!=%d), %d inodes ,%d data blocks", actual_disk_size, disk_size, total_inode_count,
//        block_count);
    disk_reset_inodes(disk);
    disk_reset_blocks(disk);
    return true;
}

void disk_free(disk_t *disk) {
    if (disk->inode_list)
        free(disk->inode_list);
    if (disk->data_block_list)
        free(disk->data_block_list);
}


int disk_find_free_inode(disk_t *disk) {
    if (disk == NULL) {
        return -2;
    }
    if (disk->sb.free_i_cnt == 0) {
        return -3;
    }
    uint32_t i_count = disk->sb.total_i_cnt;
    for (int i = 0; i < i_count; i++) {
        if (disk->inode_list[i].occupied == false) {
            return i;
        }
    }
    return -1;
}


int disk_find_free_data_block(disk_t *disk) {
    if (disk == NULL) {
        return -2;
    }
    if (disk->sb.free_b_cnt == 0) {
        return -3;
    }
    uint32_t b_count = disk->sb.total_b_cnt;
    for (int i = 0; i < b_count; i++) {
        if (disk->data_block_list[i].info.occupied == false) {
            return i;
        }
    }
    return -1;
}

int disk_assign_inode(disk_t *disk) {
    int inode_index = disk_find_free_inode(disk);
    inode_t *inode = disk_fetch_inode(disk, inode_index);
    if (inode == NULL) {
        return -1;
    }
    inode->occupied = true;
    inode->mode.file_type = Normal;
    disk->sb.free_i_cnt--;
    //set a_time
    return inode_index;
}


bool disk_save(disk_t *disk, const char *file_name) {
    if (disk == NULL) {
        ERR("get a nullptr of disk_t");
        return false;
    }

    FILE *fp = fopen(file_name, "w");
    if (fp == NULL) {
        ERR("can not open file %s", file_name);
    }

    fwrite(&disk->sb, sizeof(super_block_t), 1, fp);
    fwrite(disk->inode_list, sizeof(inode_t), disk->sb.total_i_cnt, fp);
    for (int i = 0; i < disk->sb.total_b_cnt; i++) {
        fwrite(&disk->data_block_list[i].info, sizeof(block_info_t), 1, fp);
    }

    for (int i = 0; i < disk->sb.total_b_cnt; i++)
        fwrite(disk->data_block_list[i].data, DATA_BLOCK_SIZE, disk->sb.total_b_cnt, fp);
    fclose(fp);
    return true;
}

bool disk_load(disk_t *disk, const char *file_name) {
    if (disk->data_block_list != NULL || disk->inode_list != NULL) {
        INFO("the data in disk will be erase");
    }

    FILE *fp = fopen(file_name, "r");
    if (fp == NULL) {
        ERR("can not open file %s", file_name);
        return false;
    }
    size_t cnt = fread(&disk->sb, sizeof(super_block_t), 1, fp);
    if (!super_block_valid(&disk->sb)) {
        ERR("invalid img format\n");
        return false;
    }

    disk->inode_list = (inode_t *) malloc(sizeof(inode_t) * disk->sb.total_i_cnt);
    disk->data_block_list = (data_block_t *) malloc(sizeof(data_block_t) * disk->sb.total_b_cnt);
    if (disk->inode_list == NULL || disk->data_block_list == NULL) {
        ERR("alloc memory failure\n");
        return false;
    }
    cnt = fread(disk->inode_list, sizeof(inode_t), disk->sb.total_i_cnt, fp);
    if (cnt < disk->sb.total_i_cnt) {
        ERR("can not find complete data block info\n");
        return false;

    }
    for (int i = 0; i < disk->sb.total_b_cnt; i++) {
        cnt = fread(&disk->data_block_list[i].info, sizeof(block_info_t), 1, fp);
        if (cnt != 1) {
            ERR("can not find complete data block info\n");
            return false;
        }
    }

    for (int i = 0; i < disk->sb.total_b_cnt; i++) {
        cnt = fread(&disk->data_block_list[i].data, sizeof(byte_t), DATA_BLOCK_SIZE, fp);
        if (cnt != DATA_BLOCK_SIZE) {
            ERR("can not find complete data block info\n");
            return false;
        }
    }
    LOG("success");
    fclose(fp);

    return true;
}

bool disk_visual(disk_t *disk) {
    if (disk == NULL) {
        ERR("disk is null");
        return 0;
    }
    super_block_dump(&disk->sb);
    printf("Inode:\n");
    for (int i = 0; i < disk->sb.total_i_cnt; i++) {
        inode_t inode = disk->inode_list[i];
        if (inode.occupied) {
            inode_dump(&inode);
        }
    }

    printf("--------\nData block:\n");
    for (int i = 0; i < disk->sb.total_b_cnt; i++) {
        data_block_t data_block = disk->data_block_list[i];
        if (data_block.info.occupied) {
            block_dump(&data_block);
        }
    }
    printf("==============================================================\n");
    return true;
}


