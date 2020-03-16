//
// Created by Dima Zhylko on 16/03/2020.
//

#ifndef SYSOPY_MAIN_H
#define SYSOPY_MAIN_H

#include <stddef.h>
#include <stdio.h>

int generate(char* path, size_t record_len, size_t num_records);
int sys_sort(int file, size_t record_len, int l, int r);
int sys_copy(char *path1, char *path2, size_t record_len, size_t num_records);
int lib_sort(FILE *file, size_t record_len, int l, int r);
int lib_copy(char *path1, char *path2, size_t record_len, size_t num_records);
#endif //SYSOPY_MAIN_H
