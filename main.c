#include <stdio.h>
#include "disk.h"
#include "utils.h"
#include "inode.h"
#include "io.h"

int main() {
//    disk_t disk;
//    const uint32_t size = 1024 * 128;
//    if (!disk_init(&disk, size)) {
//        return -1;
//    }
//    int index = disk_assign_inode(&disk);
//    inode_add_new_block(disk.inode_list + index, &disk);
//    inode_add_new_block(disk.inode_list + index, &disk);
//    disk_save(&disk, "disk_1.img");
//    disk_visual(&disk);

//
//    printf("-----------------------\n");
    disk_t disk2 = {};
    disk_load(&disk2, "disk_1.img");
    disk_visual(&disk2);
    return 0;
}