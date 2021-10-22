//
// Created by xhy on 2021/10/21.
//

#include "inode.h"
#include <stdio.h>

void file_mode_init(file_mode_t *file_mode) {
    file_mode->file_type = None;
    file_mode->up = 0;
    file_mode->gp = 0;
    file_mode->ap = 0;
}

void inode_init(inode_t *inode) {
    if (inode == NULL) {
        return;
    }

    inode->occupied = false;
    inode->number = -1;
    inode->file_size = 0;
    file_mode_init(&inode->mode);
    inode->uid = -1;
    inode->gid = -1;
    inode->c_time = 0;
    inode->m_time = 0;
    inode->a_time = 0;
    inode->block_used = 0;
    inode->links = 0;
    for (int i = 0; i < INODE_DIRECT_NUM; i++) {
        inode->direct[i] = -1;
    }
    inode->singly = -1;
    inode->doubly = -1;
    inode->triple = -1;
}

bool inode_dump(inode_t *inode) {
    if (inode == NULL) {
        ERR("got a nullptr of inode");
        return false;
    }
    printf("Inode[%d]\n", inode->number);
    if (inode->occupied == false) {
        INFO("not in use\n");
        return true;
    }

    printf("Blocks: %d    Size: %u   Type %d\n", inode->block_used, inode->file_size, inode->mode.file_type);
    printf("Inode number: %d    Links: %d\n", inode->number, inode->links);
    printf("Permission: (%d%d%d)   Uid: %d    Gid: %d\n", inode->mode.up, inode->mode.gp, inode->mode.ap, inode->uid,
           inode->gid);
    //todo time print
    return true;
}

