//
// Created by Dima Zhylko on 09/03/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "library.h"

void print_help(void) {
    printf("Usage:\n");
    printf("First you should create table to fit all blocks you want to create, use: ./executable create_table <size> [ other commands]\n");

    printf("List of commands with usage:\n");
    printf("create_table <size> - allocate table for size blocks\n");
    printf("compare_pairs first_pair1.txt:first_pair2.txt[ more pairs] - compare sequence of files and append to table\n");
    printf("remove_block <index> - remove block at index <index>\n");
    printf("remove_operation <block_index> <operation_index> - remove operation at index <operation_index> from block at index <block_index>\n");
}

char *available_commands[3] = {"compare_pairs", "remove_block", "remove_operation"};

int check_command(char* com){
    for(int i = 0;i<3;i++){
        if(strcmp(com, available_commands[i]) == 0){
            return i+1;
        }
    }
    return 0;
}

int main(int argc, char **argv) {

    if (argc == 2 && strcmp(argv[1], "--help")) {
        print_help();
        return 0;
    }

    if (argc < 3 || strcmp(argv[1], "create_table") != 0) {
        printf("error parsing arguments: use format ./executable create_table <size>[ other commands, use --help to check the list of available commands]");
        exit(1);
    }

    size_t table_size = strtol(argv[2], NULL, 10);

    struct main_table* table = create(table_size);

    if (table == NULL) {
        exit(1);
    }

    for (int i = 3; i < argc; i++) {
        int command = check_command(argv[i]);

        // compare pairs
        if (command == 1) {
            size_t size = 0;

            while (i+size+1 < argc && check_command(argv[i+size+1]) == 0) {
                size++;
            }

            char **pair_seq = (char **) calloc(size, sizeof(char *));

            for (int j = 0; j < size; j++) {
                pair_seq[j] = argv[i+j+1];
            }

            struct pair_sequence *seq = create_sequence(size, pair_seq);

            if (compare_pairs(table, seq) == 1) {
                exit(1);
            }

            i += size;
        } else if (command == 2) {
            if(i+1 >= argc){
                printf("error while parsing arguments: wrong usage of remove_block specify block_index");
                exit(1);
            }
            size_t index = strtol(argv[++i], NULL, 10);
            remove_block(table, index);
        } else if (command == 3){
            if(i+2 >= argc){
                printf("error while parsing arguments: wrong usage of remove_operation specify block_index and operation_index");
                exit(1);
            }
            size_t block_index = strtol(argv[++i], NULL, 10);
            size_t operation_index = strtol(argv[++i], NULL, 10);
            remove_operation(table, block_index, operation_index);
        }
    }

    return 0;
}

