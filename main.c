#include <stdio.h>
#include "disk.h"
#include "utils.h"
#include "inode.h"
#include "io.h"
#include "shell.h"
#include <stdlib.h>

void w_test() {
    disk_t disk;
    disk_init(&disk, 1024 * 1024);
    int node = disk_assign_inode(&disk);
    printf("assigned node number is %d", node);
    inode_t *inode = disk_fetch_inode(&disk, node);
    const char *str = "AQAQAQ";
    inode_write_content(inode, &disk, str, strlen(str) + 1);
    disk_visual(&disk);
    disk_save(&disk, "test2.img");
}

void r_test() {
    disk_t disk;
    bool r = disk_load(&disk, "a.img");
    if (r == false) {
        printf("disk mount failure\n");
        return;
    }
    disk_visual(&disk);
    inode_t *inode = disk_fetch_inode(&disk, 1);
    size_t len = 0;
    byte_t *byte = inode_read_all_content(inode, &disk, &len);
    if (byte != NULL) {
        printf("receive %zu byte:\n", len);
        for (int i = 0; i < len; i++) {
            printf("%c", byte[i]);
        }
    }
}


void root_dir_test() {
    const uint32_t size = 1024 * 1024;
    disk_t disk;
    if (!disk_format(&disk, size)) {
        ERR("error create disk");
    }
    size_t s = 0;
    create_empty_file(&disk, "a.c");
    create_empty_file(&disk, "a.cpp");
    create_empty_file(&disk, "a.exe");
    dir_entry_t *dl2 = disk_get_file_list(&disk, &s);
    for (int i = 0; i < s; i++) {
        printf("%s\n", dl2[i].dir_name);
    }
    disk_visual(&disk);
    free(dl2);
}

int main() {
    shell_run();
    //r_test();
    return 0;
}