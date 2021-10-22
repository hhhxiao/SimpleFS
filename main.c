#include <stdio.h>
#include "disk.h"
#include "utils.h"
#include "inode.h"
#include "io.h"

void w_test() {
    disk_t disk;
    disk_init(&disk, 1024 * 1024);
    int node = disk_assign_inode(&disk);
    printf("assigned node number is %d", node);
    inode_t *inode = disk_fetch_inode(&disk, node);
    const char *str = "Hello, File System. Hello UESTC\nThis is a simple file system write by XHY";
    inode_write_content(inode, &disk, str, strlen(str) + 1);
    disk_visual(&disk);
    disk_save(&disk, "test2.img");
}

void r_test() {
    disk_t disk;
    bool r = disk_load(&disk, "test2.img");
    if (r == false) {
        printf("disk mount failure\n");
        return;
    }
    disk_visual(&disk);
    inode_t *inode = disk_fetch_inode(&disk, 0);
    size_t len = 0;
    byte_t *byte = inode_read_all_content(inode, &disk, &len);
    if (byte != NULL) {
        printf("receive %zu byte:\n", len);
        for (int i = 0; i < len; i++) {
            printf("%c", byte[i]);
        }
    }
}

int main() {
    //w_test();
    r_test();
    return 0;
}