#include <stdio.h>
#include "disk.h"
#include "utils.h"
#include "inode.h"
#include "io.h"

int main() {
    disk_t disk;
    disk_init(&disk, 1024 * 1024);
    disk_visual(&disk);
    int node = disk_assign_inode(&disk);
    inode_t *inode = disk_fetch_inode(&disk, node);
    const char *str = "Hello, File System. Hello UESTC\nThis is a simple file system write by XHY";
    inode_write_content(inode, &disk, str, strlen(str));
    disk_visual(&disk);
    disk_save(&disk, "QAQ.img");
    return 0;
}