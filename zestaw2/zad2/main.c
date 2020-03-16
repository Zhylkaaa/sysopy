//
// Created by Dima Zhylko on 16/03/2020.
//

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>//realpath
#include <dirent.h>// stat functions
#include <time.h>
#include <sys/stat.h> //struct stat
#include <string.h>
#include <ftw.h>

static int max = 200; // maximal length of real path
int modeGlobal = 0;
int sgnGlobal = 0;
int nGlobal = -1;
int depthGlobal = INT_MAX - 1;

char *transateFileType(unsigned char type){
    switch (type)
    {
    case 6: //DT_BLK
        return "block device";
        break;
    
    case 2: //DT_CHR
        return "character device";
        break;
    
    case 4: // DT_DIR
        return "directory";
        break;
    
    case 1: //DT_FIFO
        return "named pipe (FIFO)";
        break;
    
    case 10: //DT_LNK
        return "symbolic link";
        break;

    case 8: //DT_REG
        return "regular file";
        break;
    
    case 12: //DT_SOCK
        return "socekt";
        break;
    default:
        return "unknown";
        break;
    }
}

//get acutall date and comapre check if the given one meets the time constraint
int checkTime(struct tm *checkedTime, int sgn, int n ){
    int checked = 0;
    checked = checkedTime->tm_mday + 31*checkedTime->tm_mon + 365*checkedTime->tm_year;
   
    //I have no idea why but this changes the chectedTime too
    time_t currentTime;
    struct tm *currTime;
    currentTime = time(NULL);
    currTime = localtime(&currentTime);

    //dates represented as sum of days
    int curr = 0;
    curr = currTime->tm_mday + 31*currTime->tm_mon + 365*currTime->tm_year;
    
    //printf("%d  %d\n", curr, checked);
    
    if (sgn == 0){
        if (curr - checked == n) return 1;
        else return 0;
    }
    else if(sgn == 1)
    {
        if(curr - checked > n) return 1;
        else return 0;
    }
     else if(sgn == -1)
    {
        if(curr - checked < n) return 1;
        else return 0;
    }
    return 0;
}

void printInfo(char *absPath, struct stat *stats, unsigned char type, char *modTime, char *accTime ){
    printf("Path:   %s\n", absPath);
    printf("Links count:    %hu\n", stats->st_nlink);
    printf("Type:   %s\n", transateFileType(type));
    printf("Size:   %lld\n", stats->st_size);
    printf("Modification time:  %s\n", modTime);        
    printf("Access time:    %s\n\n", accTime);
}


//mode 0 - atime or mtime wasn't used
//mode 1 - atime used
//mode 2 - mtime used
//sgn -1 means - or 1 means + if sign was specified in mtime or atime option or 0 if wasn't specified 
//n argument of atime or mtime call, means time of access or modification in n*24h period, -1 if nor mtime or atime was used
void searchUsingStat(char *dirPath, int depth, int mode, int sgn, int n){
    if(depth < 0){
        return;
    }

    DIR *dir = opendir(dirPath);

    if(dir == NULL){
        perror("Cannot open directory with opendir");
        exit(1);
    }

    struct dirent *fptr;
    struct tm *modTime;
    struct tm *accTime;
    char *absPath = (char *) calloc(max, sizeof(char));
    char *nextPath = (char *) calloc(max, sizeof(char));
    struct stat stats;

    while ((fptr = readdir(dir)) != NULL)
    {
        if (strcmp(fptr->d_name, ".") == 0 || strcmp(fptr->d_name, "..") == 0){
            continue;
        }

        strcpy(nextPath, dirPath);
        strcat(nextPath, "/");
        strcat(nextPath, fptr->d_name);
        
        realpath(nextPath, absPath);

        if(lstat(nextPath, &stats) < 0){
            perror("lstat error");
            exit(1);
        }
       
        //printf("%s\n", nextPath);
        modTime = localtime(&stats.st_mtime);
        
        char *modificationTime = (char *) calloc(40, sizeof(char));
        if (strftime(modificationTime, 40, "%d.%m.%Y", modTime) == 0){
            printf("Error during convering date to string!\n");
        }
        
        char *accessTime = (char *) calloc(40, sizeof(char));
        accTime = localtime(&stats.st_atime);
        if (strftime(accessTime, 40, "%d.%m.%Y", accTime) == 0){
            printf("Error during convering date to string!\n");
        }

        switch (mode)
        {
        case 0:
            printInfo(absPath,&stats,fptr->d_type, modificationTime, accessTime);
            break;
        
        case 1:
            if (checkTime(accTime, sgn, n) > 0){
                printInfo(absPath,&stats,fptr->d_type, modificationTime, accessTime);
            }
            break;
        
        case 2:
            if (checkTime(modTime, sgn, n) > 0){
                printInfo(absPath,&stats,fptr->d_type, modificationTime, accessTime);
            }

        default:
            break;
        }

        if(fptr->d_type == 4){ //DT_DIT
            searchUsingStat(nextPath, depth -1, mode, sgn, n) ;
        }
        free(accessTime);
        free(modificationTime);

    }
    
    free(absPath);
    free(nextPath);
    free(fptr);
    closedir(dir);
}

char *transateFileTypeNFTW(int type){
    switch (type)
    {
        case FTW_F: return "regular file";
        case FTW_D: return "directory";
        case FTW_SL: return "symbolic link";
        default: return "unknown";
    };
}

void printInfoNFTW(char *absPath, const struct stat *stats, int type, char *modTime, char *accTime ){
    printf("Path:   %s\n", absPath);
    printf("Links count:    %hu\n", stats->st_nlink);
    printf("Type:   %s\n", transateFileTypeNFTW(type));
    printf("Size:   %lld\n", stats->st_size);
    printf("Modification time:  %s\n", modTime);        
    printf("Access time:    %s\n\n", accTime);
}

int displayInfoNFTW(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
    if (ftwbuf->level > depthGlobal + 1|| strcmp(fpath, ".") == 0){
        return 0;
    }
    
    struct tm *modTime;
    struct tm *accTime;
    char *absPath = (char *) calloc(max, sizeof(char));

    modTime = localtime(&sb->st_mtime);
            
    char *modificationTime = (char *) calloc(40, sizeof(char));
    if (strftime(modificationTime, 40, "%d.%m.%Y", modTime) == 0){
         printf("Error during convering date to string!\n");
    }
            
    char *accessTime = (char *) calloc(40, sizeof(char));
    accTime = localtime(&sb->st_atime);
    if (strftime(accessTime, 40, "%d.%m.%Y", accTime) == 0){
        printf("Error during convering date to string!\n");
    }

    realpath(fpath, absPath);
    
    switch (modeGlobal)
        {
        case 0:
            printInfoNFTW(absPath, sb, tflag, modificationTime, accessTime);
            break;
        
        case 1:
            if (checkTime(accTime, sgnGlobal, nGlobal) > 0){
                printInfoNFTW(absPath, sb, tflag, modificationTime, accessTime);
            }
            break;
        
        case 2:
            if (checkTime(modTime, sgnGlobal, nGlobal) > 0){
                printInfoNFTW(absPath, sb, tflag, modificationTime, accessTime);
            }

        default:
            break;
        }
    
    free(accessTime);
    free(modificationTime);



    return 0;
}
 

void printUsage(){
    printf("usage is ./programName path [-maxdepth depth] [-atime n or -mtime n]\n");
}

void functionCallWrapper(char *path){
    printf("Using stat: \n\n");
    searchUsingStat(path, depthGlobal, modeGlobal, sgnGlobal, nGlobal);

    printf("Using nftw: \n\n");
    nftw(path, displayInfoNFTW, 10, FTW_PHYS);
    

}

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Please specify directory to be searched\n");
        return 1;
    }

    // ./main path
    if (argc == 2){
        functionCallWrapper(argv[1]);
    }
    else if (argc == 4)
    {
        // ./main path -maxdepth n
        if(strcmp(argv[2], "-maxdepth") == 0){
            depthGlobal = atoi(argv[3]);
            
            functionCallWrapper(argv[1]);
        }
        
        else if(strcmp(argv[2], "-atime") == 0){
            modeGlobal = 1;

            // ./main path -atime +n 
            if (argv[3][0] == 43){ // 43 is + in ASCII
                char *ptr = (char *) (&argv[3][1]);
                nGlobal = atoi(ptr);
                sgnGlobal = 1;
                
                functionCallWrapper(argv[1]);
            }
            // ./main path -atime -n
            else if(argv[3][0] == 45) // 45 is - in ASCII
            {
                char *ptr = (char *) (&argv[3][1]);
                nGlobal = atoi(ptr);
                sgnGlobal = -1;

                functionCallWrapper(argv[1]);
            }
            
            // ./main path -atime n
            else
            {
                nGlobal = atoi(argv[3]);
                sgnGlobal = 0;
                
                functionCallWrapper(argv[1]);
            }
            
        }

        else if(strcmp(argv[2], "-mtime") == 0){
            modeGlobal = 2;
            // ./main path -mtime +n
            if (argv[3][0] == 43){ // 43 is + in ASCII
                char *ptr = (char *) (&argv[3][1]);
                nGlobal = atoi(ptr);
                sgnGlobal = 1;
                
                functionCallWrapper(argv[1]);
            }
            // ./main path -mtime -n
            else if(argv[3][0] == 45) // 45 is - in ASCII
            {
                char *ptr = (char *) (&argv[3][1]);
                nGlobal = atoi(ptr);
                sgnGlobal = -1;

                functionCallWrapper(argv[1]);
            }
            // ./main path -mtime n
            else
            {
                nGlobal = atoi(argv[3]);
                sgnGlobal = 0;
                
                functionCallWrapper(argv[1]);
            }
            
        }
        //input error - no such option
        else
        {
            printf("There is no such option\n");
            printUsage();
            return 1;
        }
        
    
    }

    else if (argc == 6){
        //such as in find maxdepth should be before atime or mtime
        if(strcmp(argv[2], "-maxdepth") == 0){
            depthGlobal = atoi(argv[3]);
            
            if(strcmp(argv[4], "-atime") == 0){
                modeGlobal = 1;
                // ./main path -maxdepth n -atime +m
                if (argv[5][0] == 43){ // 43 is + in ASCII
                    char *ptr = (char *) (&argv[5][1]);
                    nGlobal = atoi(ptr);
                    sgnGlobal = 1;
                    
                    functionCallWrapper(argv[1]);
                }
                // ./main path -maxdepth n -atime -m
                else if(argv[5][0] == 45) // 45 is - in ASCII
                {
                    char *ptr = (char *) (&argv[5][1]);
                    nGlobal = atoi(ptr);
                    sgnGlobal = -1;

                    functionCallWrapper(argv[1]);
                }
                // ./main path -maxdepth n -atime m
                else
                {
                    nGlobal = atoi(argv[5]);
                    sgnGlobal = 0;
                    
                    functionCallWrapper(argv[1]);
                }
                
            }
            else if(strcmp(argv[4], "-mtime") == 0){
                modeGlobal = 2;
                // ./main path -maxdepth n -mtime +m
                if (argv[5][0] == 43){ // 43 is + in ASCII
                    char *ptr = (char *) (&argv[5][1]);
                    nGlobal = atoi(ptr);
                    sgnGlobal = 1;
                    
                    functionCallWrapper(argv[1]);
                }
                // ./main path -maxdepth n -mtime -m
                else if(argv[5][0] == 45) // 45 is - in ASCII
                {
                    char *ptr = (char *) (&argv[5][1]);
                    nGlobal = atoi(ptr);
                    sgnGlobal = -1;

                    functionCallWrapper(argv[1]);
                }
                // ./main path -maxdepth n -mtime m
                else
                {
                    nGlobal = atoi(argv[5]);
                    sgnGlobal = 0;
                    
                    functionCallWrapper(argv[1]);
                }
                
            }
            else
            {
                printf("There is no such option\n");
                printUsage();
                return 1;
            }
            }
        else
        {
            printUsage();
            return 1;
        }
        
    }
    else
    {
        printUsage();
        return 1;
    }
    
    return 0;
}