// THIS FILE BELONGS TO TWO LOCATIONS: 
// 1. kernel/inc/stacsos/kernel/fs/dirent.h
// 2. user/ulib/inc/stacsos/dirent.h

#pragma once

namespace stacsos {

struct dirent {
    char name[256]; // Max filename length
    int kind; // 0 = File, 1 = Directory
    unsigned long size; // File size
} __attribute__((packed));;
}