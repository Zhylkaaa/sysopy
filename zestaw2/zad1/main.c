//
// Created by Dima Zhylko on 16/03/2020.
//

#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <time.h>
#include <sys/times.h>

#define SYS_MODE 1
#define LIB_MODE 0
#define UNKNOWN_MODE 2

int generate(char *path, size_t record_len, size_t num_records) {
    FILE *file = fopen(path, "w+");
    FILE *random = fopen("/dev/random", "r");

    char *rand_str = calloc(record_len + 1, sizeof(char));

    for (int i = 0; i < num_records; i++) {
        if (fread(rand_str, sizeof(char), record_len, random) != record_len) {
            return 1;
        }

        for (int j = 0; j < record_len; j++) {
            if (rand_str[j] == -128) {
                rand_str[j] += 1;
            }

            rand_str[j] = abs(rand_str[j]);
            rand_str[j] = ((rand_str[j] % 52) + 'A' + (-'A' - 26 + 'a') * ((rand_str[j] % 52) > 25));
        }

        rand_str[record_len] = '\n';

        if (fwrite(rand_str, sizeof(char), record_len + 1, file) != record_len + 1) {
            return 1;
        }
    }

    fclose(file);
    fclose(random);
    free(rand_str);
    return 0;
}

int lib_sort(FILE *file, size_t record_len, int l, int r) {
    if (r - l < 2) {
        return 0;
    }

    char *reg1 = (char *) calloc(record_len + 1, sizeof(char));
    char *reg2 = (char *) calloc(record_len + 1, sizeof(char));

    long int offset = record_len + 1;

    fseek(file, offset * (r - 1), 0); // find pivot
    if (fread(reg1, sizeof(char), offset, file) != offset) {
        free(reg1);
        free(reg2);
        return 1;
    }

    int j = l - 1;

    for (int i = l; i < r - 1; i++) {

        fseek(file, offset * i, 0);
        if (fread(reg2, sizeof(char), offset, file) != offset) {
            free(reg1);
            free(reg2);
            return 1;
        }


        if (reg1[0] > reg2[0]) {
            j++;
            fseek(file, j * offset, 0);
            if (fread(reg1, sizeof(char), offset, file) != offset) {
                free(reg1);
                free(reg2);
                return 1;
            }

            fseek(file, j * offset, 0);
            if (fwrite(reg2, sizeof(char), offset, file) != offset) {
                free(reg1);
                free(reg2);
                return 1;
            }

            fseek(file, i * offset, 0);
            if (fwrite(reg1, sizeof(char), offset, file) != offset) {
                free(reg1);
                free(reg2);
                return 1;
            }

            fseek(file, offset * (r - 1), 0); // find pivot
            if (fread(reg1, sizeof(char), offset, file) != offset) {
                free(reg1);
                free(reg2);
                return 1;
            }
        }
    }

    j++;

    fseek(file, j * offset, 0);
    if (fread(reg2, sizeof(char), offset, file) != offset) {
        free(reg1);
        free(reg2);
        return 1;
    }

    fseek(file, j * offset, 0);
    if (fwrite(reg1, sizeof(char), offset, file) != offset) {
        free(reg1);
        free(reg2);
        return 1;
    }

    fseek(file, (r - 1) * offset, 0);
    if (fwrite(reg2, sizeof(char), offset, file) != offset) {
        free(reg1);
        free(reg2);
        return 1;
    }

    free(reg1);
    free(reg2);

    if (lib_sort(file, record_len, l, j) != 0) {
        return 1;
    }
    if (lib_sort(file, record_len, j + 1, r) != 0) {
        return 1;
    }

    return 0;
}

int sys_sort(int file, size_t record_len, int l, int r) {
    if (r - l < 2) {
        return 0;
    }

    char *reg1 = (char *) calloc(record_len + 1, sizeof(char));
    char *reg2 = (char *) calloc(record_len + 1, sizeof(char));

    size_t offset = (record_len + 1) * sizeof(char);

    lseek(file, offset * (r - 1), SEEK_SET); // find pivot
    if (read(file, reg1, offset) != offset) {
        free(reg1);
        free(reg2);
        return 1;
    }

    int j = l - 1;

    for (int i = l; i < r - 1; i++) {

        lseek(file, offset * i, SEEK_SET);
        if (read(file, reg2, offset) != offset) {
            free(reg1);
            free(reg2);
            return 1;
        }


        if (reg1[0] > reg2[0]) {
            j++;
            lseek(file, j * offset, SEEK_SET);
            if (read(file, reg1, offset) != offset) {
                free(reg1);
                free(reg2);
                return 1;
            }

            lseek(file, j * offset, SEEK_SET);
            if (write(file, reg2, offset) != offset) {
                free(reg1);
                free(reg2);
                return 1;
            }

            lseek(file, i * offset, SEEK_SET);
            if (write(file, reg1, offset) != offset) {
                free(reg1);
                free(reg2);
                return 1;
            }

            lseek(file, offset * (r - 1), SEEK_SET); // find pivot
            if (read(file, reg1, offset) != offset) {
                free(reg1);
                free(reg2);
                return 1;
            }
        }
    }

    j++;

    lseek(file, j * offset, SEEK_SET);
    if (read(file, reg2, offset) != offset) {
        free(reg1);
        free(reg2);
        return 1;
    }

    lseek(file, j * offset, SEEK_SET);
    if (write(file, reg1, offset) != offset) {
        free(reg1);
        free(reg2);
        return 1;
    }

    lseek(file, (r - 1) * offset, SEEK_SET);
    if (write(file, reg2, offset) != offset) {
        free(reg1);
        free(reg2);
        return 1;
    }

    free(reg1);
    free(reg2);

    if (sys_sort(file, record_len, l, j) != 0) {
        return 1;
    }
    if (sys_sort(file, record_len, j + 1, r) != 0) {
        return 1;
    }

    return 0;
}

void sort(char *path, size_t record_len, size_t num_records, int mode) {
    if (mode == LIB_MODE) {
        FILE *file = fopen(path, "r+");
        if (lib_sort(file, record_len, 0, num_records) != 0) {
            printf("Error occurred while sorting\n");
            exit(1);
        }
        fclose(file);

    } else if (mode == SYS_MODE) {
        int file = open(path, O_RDWR);
        if (sys_sort(file, record_len, 0, num_records) != 0) {
            printf("Error occurred while sorting\n");
            exit(1);
        }
        close(file);
    } else {
        printf("Invalid argument");
    }
}


int sys_copy(char *path1, char *path2, size_t record_len, size_t num_records) {
    char buf[record_len + 1];
    int file1 = open(path1, O_RDONLY);
    int file2 = open(path2, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);

    size_t size = (record_len + 1) * sizeof(char);

    for (int i = 0; i < num_records; i++) {
        int bt = read(file1, buf, size);

        if (bt != size || bt == 0) {
            close(file1);
            close(file2);
            return 1;
        }

        if (write(file2, buf, size) != size) {
            close(file1);
            close(file2);
            return 1;
        }
    }
    close(file1);
    close(file2);
    return 0;
}

int lib_copy(char *path1, char *path2, size_t record_len, size_t num_records) {
    char buf[record_len + 1];

    FILE *file1 = fopen(path1, "r");
    FILE *file2 = fopen(path2, "w");

    size_t size = record_len + 1;

    for (int i = 0; i < num_records; i++) {
        int bt = fread(buf, sizeof(char), size, file1);

        if (bt != size || bt == 0) {
            fclose(file1);
            fclose(file2);
            return 1;
        }

        if (fwrite(buf, sizeof(char), size, file2) != size) {
            fclose(file1);
            fclose(file2);
            return 1;
        }
    }
    fclose(file1);
    fclose(file2);
    return 0;
}

void copy(char *path1, char *path2, size_t record_len, size_t num_records, int mode) {
    if (mode == LIB_MODE) {
        if (lib_copy(path1, path2, record_len, num_records) != 0) {
            printf("Error occurred while copying\n");
            exit(1);
        }
    } else if (mode == SYS_MODE) {
        if (sys_copy(path1, path2, record_len, num_records) != 0) {
            printf("Error occurred while copying\n");
            exit(1);
        }
    } else {
        printf("Invalid argument\n");
        exit(1);
    }
}

double calculate_time(clock_t start, clock_t end){
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("Wrong number of arguments\n");
        exit(1);
    }

    if (strcmp(argv[1], "generate") == 0) {
        if (argc != 5) {
            printf("Wrong number of arguments\n");
            exit(1);
        }

        if (generate(argv[2], atoi(argv[3]), atoi(argv[4])) != 0) {
            printf("Error occurred while generating\n");
            exit(1);
        }

        return 0;
    }

    if (strcmp(argv[1], "sort") == 0) {
        if (argc != 6) {
            printf("Wrong number of arguments\n");
            exit(1);
        }


        struct tms** tms_time = calloc(2, sizeof(struct tms*));
        tms_time[0] = malloc(sizeof(struct tms));
        tms_time[1] = malloc(sizeof(struct tms));
        clock_t real_time[2];

        if (strcmp(argv[5], "sys") == 0) {
            real_time[0] = times(tms_time[0]);
            sort(argv[2], atoi(argv[3]), atoi(argv[4]), SYS_MODE);
            real_time[1] = times(tms_time[1]);
        } else if (strcmp(argv[5], "lib") == 0) {
            real_time[0] = times(tms_time[0]);
            sort(argv[2], atoi(argv[3]), atoi(argv[4]), LIB_MODE);
            real_time[1] = times(tms_time[1]);
        } else {
            sort(argv[2], atoi(argv[3]), atoi(argv[4]), UNKNOWN_MODE);
        }

        printf("REAL        USER       SYSTEM\n");
        printf("%lf    %lf   %lf\n\n", calculate_time(real_time[0], real_time[1]),
                calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime), calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));
        return 0;
    }

    if (strcmp(argv[1], "copy") == 0) {
        if (argc != 7) {
            printf("Wrong number of arguments\n");
            exit(1);
        }

        struct tms** tms_time = calloc(2, sizeof(struct tms*));
        tms_time[0] = malloc(sizeof(struct tms));
        tms_time[1] = malloc(sizeof(struct tms));
        clock_t real_time[2];

        if (strcmp(argv[6], "sys") == 0) {
            real_time[0] = times(tms_time[0]);
            copy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), SYS_MODE);
            real_time[1] = times(tms_time[1]);
        } else if (strcmp(argv[6], "lib") == 0) {
            real_time[0] = times(tms_time[0]);
            copy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), LIB_MODE);
            real_time[1] = times(tms_time[1]);
        } else {
            copy(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), UNKNOWN_MODE);
        }

        printf("REAL        USER       SYSTEM\n");
        printf("%lf    %lf   %lf\n\n", calculate_time(real_time[0], real_time[1]),
               calculate_time(tms_time[0]->tms_utime, tms_time[1]->tms_utime), calculate_time(tms_time[0]->tms_stime, tms_time[1]->tms_stime));

        return 0;
    }

    printf("Unknown command %s\n", argv[2]);
    exit(1);
}