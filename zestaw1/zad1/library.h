//
// Created by Dima Zhylko on 08/03/2020.
//

#ifndef SYSOPY_LIBRARY_H
#define SYSOPY_LIBRARY_H

#include <stddef.h>

struct pair_of_files {
    char* first_path;
    char* second_path;
};

struct pair_sequence {
    struct pair_of_files* sequence;
    size_t size;
};

struct block {
    char** operations;
    size_t size;
};

struct main_table {
    struct block** blocks;
    size_t size;
    size_t last_index;
};

struct main_table* create(size_t size);
int create_pair_of_files(char* str_pair, struct pair_of_files* pair); // returns 0 if operation succeed 1 otherwise
struct pair_sequence* create_sequence(int num, char **pairs);
int compare_and_write_to_file(struct pair_of_files* pair, char* tmp_name); // returns 0 if operation succeed 1 otherwise
struct block* create_block(char* tmp_name);
int add_block(struct main_table* table, char* tmp_name); // returns index of last added block or -1 if operation failed
int compare_pairs(struct main_table* table, struct pair_sequence* sequence); // returns 0 if operation succeed 1 otherwise
size_t get_block_size(struct main_table* table, size_t index);
int remove_block(struct main_table* table, size_t index); // returns 0 if operation succeed 1 if index doesn't exists
int remove_operation(struct main_table* table, size_t block_index, size_t operation_index); // returns 0 if operation succeed 1 if index doesn't exists

#endif //SYSOPY_LIBRARY_H
