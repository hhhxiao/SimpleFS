//
// Created by xhy on 2021/10/21.
//

#include "io.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

inode_pointer_t *inode_get_pointer(inode_t *inode, uint32_t index, disk_t *disk) {
    if (inode == NULL || disk == NULL || index < 0) {
        return NULL;
    }
    if (index >= MAX_BLOCK_DIRECT + MAX_BLOCK_SIGNY + MAX_BLOCK_DOUBLY) {
        return NULL;
    }
    int max_used_block = MAX_BLOCK_DIRECT;
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


uint32_t inode_get_last_block(inode_t *inode, disk_t *disk) {
    return inode_get_block(inode, inode->block_used - 1, disk);
}

uint32_t inode_add_new_block(inode_t *inode, disk_t *disk) {
    if (inode == NULL) {
        return -1;
    }
    inode_pointer_t *p = inode_get_pointer(inode, inode->block_used, disk);
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

int inode_write_content(inode_t *inode, disk_t *disk, const byte_t *byte, size_t len) {
    if (inode == NULL || disk == NULL) {
        return -1;
    }
    const static size_t max_file_size =
            (MAX_BLOCK_DIRECT +
             MAX_BLOCK_SIGNY * INODE_PTR_NUM_IN_BLOCK +
             MAX_BLOCK_DOUBLY * INODE_PTR_NUM_IN_BLOCK * INODE_PTR_NUM_IN_BLOCK) * DATA_BLOCK_SIZE;
    if (len + inode->file_size > max_file_size) {
        ERR("content is too large to write\n");
        return -2;
    }

    int write_len = 0;
    int last_block_idx = (int) inode_get_last_block(inode, disk);
    //一个块都没分配
    if (last_block_idx == -1) {
        last_block_idx = (int) inode_add_new_block(inode, disk);
    }
    while (write_len < len) {
        data_block_t *last_block = disk_fetch_block(disk, last_block_idx);
        assert(last_block != NULL);
        //向数据块写数据
        write_len += block_write_content(last_block, byte + write_len, len - write_len);
        if (write_len >= len) {
            break;
        }
        last_block_idx = (int) inode_add_new_block(inode, disk);
        if (last_block_idx == -1) {
            ERR("disk space is not enough");
            inode->file_size += write_len;
            return false;
        }
    }

    inode->file_size += write_len;
    return true;
}


byte_t *inode_read_all_content(inode_t *inode, disk_t *disk, size_t *len) {
    const size_t file_size = inode->file_size;
    *len = file_size;
    byte_t *byte = (byte_t *) malloc(file_size * sizeof(byte));
    int block_idx = 0;
    int w_idx = 0;
    while (block_idx < inode->block_used) {
        inode_pointer_t bid = inode_get_block(inode, block_idx, disk);
        if (bid == -1) {
            ERR("disk was destroyed\n");
            return NULL;
        }
        data_block_t *b = disk_fetch_block(disk, (int) bid);
        if (b == NULL || b->info.occupied == false) {
            ERR("disk was destroyed\n");
            return NULL;
        }
        for (int i = 0; i < b->info.space_used; i++) {
            byte[i + w_idx] = b->data[i];
        }
        w_idx += b->info.space_used;
        ++block_idx;
    }
    return byte;

}


bool create_root_dir(disk_t *disk) {
    inode_t *inode = disk_fetch_inode(disk, ROOT_DIR_INODE);
    if (inode == NULL || inode->occupied == true) {
        ERR("can not create root dir");
        return false;
    }
    inode->occupied = true;
    inode->mode.file_type = Dir;
    dir_entry_t dir_entry;
    dir_entry_init(&dir_entry, ".", 0);
    inode_write_content(inode, disk, (byte_t *) &dir_entry, sizeof(dir_entry));
    dir_entry_init(&dir_entry, "..", 0);
    inode_write_content(inode, disk, (byte_t *) &dir_entry, sizeof(dir_entry));
    inode->cnt_file_num += 2;
    return true;
}

bool disk_format(disk_t *disk, uint32_t size) {
    return disk_init(disk, size) && create_root_dir(disk);
}

dir_entry_t *disk_get_file_list(disk_t *disk, size_t *num) {
    inode_t *inode = disk_fetch_inode(disk, ROOT_DIR_INODE);
    size_t len = 0;
    byte_t *b = inode_read_all_content(inode, disk, &len);
    uint32_t cnt = len / sizeof(dir_entry_t);
    assert(cnt == inode->cnt_file_num);
    assert(len % sizeof(dir_entry_t) == 0);
    *num = cnt;
    return (dir_entry_t *) b;
}


bool create_empty_file(disk_t *disk, const char *file_name) {
    if (disk == NULL) {
        return false;
    }
    size_t num = 0;
    dir_entry_t *dir = disk_get_file_list(disk, &num);
    for (int i = 0; i < num; i++) {
        if (strcmp(file_name, dir[num].dir_name) == 0) {
            ERR("file already exists");
            return false;
        }
    }

    //assign inode
    int node = disk_assign_inode(disk);
    dir_entry_t entry;
    if (node > 0 && dir_entry_init(&entry, file_name, node)) {
        inode_t *root_node = disk_fetch_inode(disk, ROOT_DIR_INODE);
        if (inode_write_content(root_node, disk, (byte_t *) &entry,
                                sizeof(dir_entry_t))) {
            root_node->cnt_file_num++;
            inode_t *file_node = disk_fetch_inode(disk, node);
            file_node->mode.file_type = Normal;
            return true;
        }
    }
    free(dir);
    return false;
}