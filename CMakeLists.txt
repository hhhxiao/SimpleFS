cmake_minimum_required(VERSION 3.14)
project(SimFS)
set(CMAKE_C_STANDARD 99)
include_directories(.)
add_executable(
        SimFS
        main.c
        utils.c
        disk.c
        block.c
        config.h
        inode.c
        io.c io.h
        shell.c
        shell.h)

