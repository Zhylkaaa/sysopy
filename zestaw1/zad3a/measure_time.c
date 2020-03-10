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
#include <sys/times.h>
#include <time.h>

void delete_n_bloks(struct main_table* table, int num){
    for(int i = 0;i<num;i++){
        remove_block(table, i);
    }
}

void add_and_delete_blocks(struct main_table* table, struct pair_sequence* seq, int num){
    for(int i = 0;i<num;i++){
        add_block(table, "tmp_name");
        remove_block(table, num+i);
    }
}

double calculate_time(clock_t start, clock_t end){
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char** argv){
    printf("tests for %s dataset\n", argv[1]);

    int num = argc - 2;
    struct pair_sequence* seq = create_sequence(num, argv+2);
    struct main_table* table = create(2*num + 2);

    struct tms **tms_time = calloc(6, sizeof(struct tms *));
    clock_t real_time[6];
    for (int i = 0; i < 6; i++) {
        tms_time[i] = (struct tms *) malloc(sizeof(struct tms));
    }

    real_time[0] = times(tms_time[0]);
    compare_pairs(table, seq);
    real_time[1] = times(tms_time[1]);

    size_t block_size =  get_block_size(table, 0);

    real_time[2] = times(tms_time[2]);
    delete_n_bloks(table, num);
    real_time[3] = times(tms_time[3]);

    real_time[4] = times(tms_time[4]);
    add_and_delete_blocks(table, seq, num);
    real_time[5] = times(tms_time[5]);


    printf("for block size %zu\n", block_size);

    printf("  REAL    User    System\n");

    printf("compare %d pairs\n", num);
    printf("%lf   ", calculate_time(real_time[0], real_time[1]));
    printf("%lf   ", calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime));
    printf("%lf   \n", calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

    printf("delete block mean\n");
    printf("%lf   ", calculate_time(real_time[2], real_time[3]) / num);
    printf("%lf   ", calculate_time(tms_time[2]->tms_utime, tms_time[3]->tms_utime) / num );
    printf("%lf   \n", calculate_time(tms_time[2]->tms_stime, tms_time[3]->tms_stime) / num);


    printf("add and delete mean\n");
    printf("%lf   ", calculate_time(real_time[4], real_time[5]) / num);
    printf("%lf   ", calculate_time(tms_time[4]->tms_utime, tms_time[5]->tms_utime) / num );
    printf("%lf   \n", calculate_time(tms_time[4]->tms_stime, tms_time[5]->tms_stime) / num);
    printf("\n\n");
}