//
// Created by Dima Zhylko on 08/03/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "library.h"

struct main_table *create(size_t size) {
    if (size < 0) {
        return NULL;
    }

    struct main_table *table = (struct main_table *) calloc(1, sizeof(struct main_table));
    if (table == NULL) {
        printf("Can't allocate main table\n");
        return NULL;
    }

    table->size = size;

    table->blocks = (struct block **) calloc(size, sizeof(struct block *));

    return table;
}

int create_pair_of_files(char *str_pair, struct pair_of_files *pair) {
    char *pos = strchr(str_pair, ':');
    if (pos == NULL) {
        printf("can't parse pair: wrong format. use format <path_to_file1>:<path_to_file2>");
        return 1;
    }

    int len_of_first = pos - str_pair;

    str_pair[len_of_first] = '\0';

    //TODO: check if allocated properly
    pair->first_path = (char *) calloc(len_of_first + 1, sizeof(char));
    pair->second_path = (char *) calloc(strlen(pos + 1) + 1, sizeof(char));

    strcpy(pair->first_path, str_pair);
    strcpy(pair->second_path, pos + 1);

    str_pair[len_of_first] = ':';

    return 0;
}

struct pair_sequence *create_sequence(int num, char **pairs) {

    struct pair_sequence *sequence = (struct pair_sequence *) calloc(1, sizeof(struct pair_sequence));
    sequence->sequence = (struct pair_of_files *) calloc(num, sizeof(struct pair_of_files));
    sequence->size = num;

    for (int i = 0; i < num; i++) {
        if (create_pair_of_files(pairs[i], &sequence->sequence[i]) != 0) {
            printf("error occurred while parsing pairs");
            for (int j = 0; j < i; j++) {
                free(sequence->sequence[i].first_path);
                free(sequence->sequence[i].second_path);
            }
            free(sequence->sequence);
            free(sequence);
            return NULL;
        }
    }

    return sequence;
}

int compare_and_write_to_file(struct pair_of_files *pair, char *tmp_name) {
    size_t size = strlen(pair->first_path) + strlen(pair->second_path) + strlen(tmp_name) + 10;
    char command[size];
    int exit_code;

    exit_code = snprintf(command, size, "diff %s %s > %s", pair->first_path, pair->second_path, tmp_name);

    if (exit_code >= 0) {
        printf("error occurred while trying to create construct diff command");
        return 1;
    }

    int diff_code = system(command);

    if (diff_code == -1 || WEXITSTATUS(diff_code) > 1) {
        printf("error occurred while executing diff\n");
        return 1;
    }

    return 0;
}

struct block *create_block(char *tmp_name) {

    FILE *handle = fopen(tmp_name, "r");
    if (!handle) {
        printf("error occurred while fopen tmp file: %s", tmp_name);
        return NULL;
    }

    //TODO: check if memory is allocated
    struct block *diff_block = (struct block *) calloc(1, sizeof(struct block));
    diff_block->size = 0;
    diff_block->operations = NULL;

    char *line_buffer = NULL;
    size_t line_buffer_size = 0;
    char *operation = (char *) malloc(0);
    size_t operation_allocated_size = 0; // total allocated size of operation
    size_t operation_len = 0; // size of actual string in operation (including null char)

    while (getline(&line_buffer, &line_buffer_size, handle) >= 0) {
        // start of new block (I hope that diff output always start with digit to indicate line)
        if (isdigit(line_buffer[0])) {
            diff_block->size++;
            if (diff_block->size == 1) {
                diff_block->operations = (char **) calloc(1, sizeof(char *));
            } else {
                diff_block->operations = (char **) realloc(diff_block->operations, diff_block->size);
            }

            diff_block->operations[diff_block->size - 1] = (char *) malloc(operation_len);
            strcpy(diff_block->operations[diff_block->size - 1], operation);

            // clear buffer
            for (int i = 0; i < operation_len; i++) {
                operation[i] = '\0';
            }
        } else { // continuation of old block
            size_t line_buffer_len = strlen(line_buffer);
            if (operation_allocated_size - operation_len < line_buffer_len) {
                if (operation_allocated_size == 0) {
                    operation_len = operation_allocated_size = line_buffer_len + 1;
                } else { // you can probably allocate this memory in more intelligent way but what the fuck are you doing hear than?
                    operation_allocated_size += line_buffer_len;
                    operation_len += line_buffer_len;
                }
                operation = (char *) realloc(operation, operation_allocated_size);
            }
            strcat(operation, line_buffer);
        }
    }

    //last operation if exists
    if (operation_allocated_size > 0) {
        diff_block->size++;
        if (diff_block->size == 1) {
            diff_block->operations = (char **) calloc(1, sizeof(char *));
        } else {
            diff_block->operations = (char **) realloc(diff_block->operations, diff_block->size);
        }

        diff_block->operations[diff_block->size - 1] = (char *) malloc(operation_len);
        strcpy(diff_block->operations[diff_block->size - 1], operation);
    }

    free(line_buffer);
    free(operation);

    return diff_block;
}

int add_block(struct main_table *table, char *tmp_name) {
    struct block *block_to_add = create_block(tmp_name);
    if (block_to_add == NULL) {
        return -1;
    }

    if (table->last_index == table->size - 1) {
        printf("can't add more blocks: table size reached");
        return -1;
    }

    table->blocks[++table->last_index] = block_to_add;
    return table->last_index;
}

int compare_pairs(struct main_table *table, struct pair_sequence *sequence) {
    char *tmp_name = "tmp_name";
    for (int i = 0; i < sequence->size; i++) {
        if (compare_and_write_to_file(&sequence->sequence[i], tmp_name) != 0) {
            return 1;
        }

        add_block(table, tmp_name);
    }
    return 0;
}

size_t get_block_size(struct main_table *table, size_t index) {
    if (!table || index > table->size || !table->blocks[index]) {
        return -1;
    }

    return table->blocks[index]->size;
}

int remove_operation(struct main_table *table, size_t block_index, size_t operation_index) {
    if (!table || block_index > table->size || !table->blocks[block_index] ||
        operation_index > get_block_size(table, block_index)
        || !table->blocks[block_index]->operations[operation_index]) {
        return 1;
    }

    free(table->blocks[block_index]->operations[operation_index]);
    table->blocks[block_index]->operations[operation_index] = NULL;
    return 0;
}

int remove_block(struct main_table *table, size_t index) {
    if (!table || index > table->size || !table->blocks[index]) {
        return 1;
    }

    for (int i = 0; i < get_block_size(table, index); i++) {
        remove_operation(table, index, i);
    }
    free(table->blocks[index]);
    table->blocks[index] = NULL;

    return 0;
}

