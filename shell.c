//
// Created by xhy on 2021/10/23.
//

#include "shell.h"
#include "utils.h"
#include "disk.h"
#include "io.h"
#include <stdlib.h>
#include <assert.h>

#define DISK_INIT_CHECK if (!init) {printf("Disk is not init\n");continue;}
#define TOKEN_NULL_CHECK(token) if(token==NULL){ fprintf(stderr,"Invalid parameter\n");fflush(stderr); continue;}
disk_t disk;
bool init = false;

void shell_run() {
    printf("Simple File System Simulator V0.0.1 By XHY\n");
    char str[512];
    while (true) {
        printf(">>> ");
        fgets(str, 512, stdin);
        str[strlen(str) - 1] = 0;//remove '\n'
        if (strcmp(str, "quit") == 0) {
            printf("Bye");
            break;
        }
        char *token = strtok(str, " ");
        TOKEN_NULL_CHECK(token)
        if (strcmp(token, "init") == 0) {
            if (init)
                INFO("the current disk will be removed\n");
            token = strtok(NULL, " ");
            TOKEN_NULL_CHECK(token)
            size_t sz = strtoul(token, NULL, 10);
            if (sz > 0 && disk_format(&disk, sz * 1024)) {
                init = true;
                INFO("init disk success\n");
            } else {
                ERR("init disk failure\n");
            }
        } else if (strcmp(token, "touch") == 0) {
            DISK_INIT_CHECK
            token = strtok(NULL, " ");
            TOKEN_NULL_CHECK(token)
            create_empty_file(&disk, token);
        } else if (strcmp(token, "ls") == 0) {
            DISK_INIT_CHECK
            size_t num;
            dir_entry_t *dir = disk_get_file_list(&disk, &num);
            printf("mode  Uid  Gid Block Size Name\n");
            for (int i = 0; i < num; i++) {
                inode_t *inode = disk_fetch_inode(&disk, (int) dir[i].inode_number);
                assert(inode);
                printf("%c%d%d%d  %-3d  %-3d   %-2d  %-2dB  %s\n",
                       inode->mode.file_type,
                       inode->mode.up,
                       inode->mode.gp,
                       inode->mode.ap,
                       inode->uid,
                       inode->gid,
                       inode->block_used,
                       inode->file_size,
                       dir[i].dir_name
                );
            }
            free(dir);
        } else if (strcmp(token, "info") == 0) {
            DISK_INIT_CHECK
            disk_visual(&disk);
        } else if (strcmp(token, "write") == 0) {

            DISK_INIT_CHECK
            token = strtok(NULL, " ");
            TOKEN_NULL_CHECK(token)
            size_t num;
            dir_entry_t *dir = disk_get_file_list(&disk, &num);
            int inode_number = -1;
            for (int i = 0; i < num; i++) {
                if (strcmp(dir[i].dir_name, token) == 0) {
                    LOG("find file");
                    inode_number = (int) dir[i].inode_number;
                    break;
                }
            }
            if (inode_number != -1) {
                byte_t *byte = malloc(sizeof(byte_t) * 128);
                memset(byte, 0, 128);
                fgets(byte, 128, stdin);
                str[strlen(str) - 1] = 0;//remove '\n'
                inode_write_content(disk_fetch_inode(&disk, inode_number), &disk, byte, strlen(byte));
                free(byte);
            } else {
                ERR("Write Error!\n");
            }
        } else if (strcmp(token, "cat") == 0) {
            DISK_INIT_CHECK
            token = strtok(NULL, " ");
            TOKEN_NULL_CHECK(token)
            size_t num;
            dir_entry_t *dir = disk_get_file_list(&disk, &num);
            int inode_number = -1;
            for (int i = 0; i < num; i++) {
                if (strcmp(dir[i].dir_name, token) == 0) {
                    LOG("find file");
                    inode_number = (int) dir[i].inode_number;
                    break;
                }
            }
            if (inode_number < 0) {
                ERR("File not found\n");
                continue;
            }
            byte_t *b = inode_read_all_content(disk_fetch_inode(&disk, inode_number), &disk, &num);
            if (b) {
                for (int i = 0; i < num; i++) {
                    printf("%c", b[i]);
                }
                printf("\n");
                free(b);
            } else {
                ERR("Read Error\n");
            }

        } else if (strcmp(token, "load") == 0) {
            if (init)
                INFO("the current disk will be removed\n");
            token = strtok(NULL, " ");
            TOKEN_NULL_CHECK(token)
            if (disk_load(&disk, token)) {
                init = true;
                INFO("Success\n");
            } else {
                ERR("Error when load disk img\n");
            }
        } else if (strcmp(token, "save") == 0) {
            DISK_INIT_CHECK
            token = strtok(NULL, " ");
            TOKEN_NULL_CHECK(token)
            if (disk_save(&disk, token)) {
                INFO("Success\n");
            } else {
                ERR("Error when save disk\n");
            }
        } else {
            ERR("Invalid Command");
        }

    }
}