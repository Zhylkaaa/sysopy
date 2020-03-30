//
// Created by Dima Zhylko on 30/03/2020.
//

#include "defines.h"

struct option {
    const char* option;
    const char* name;
};

int main(){
    const struct option options[] = {
            {.option = "i", .name="ignore"},
            {.option = "h", .name="handle"}, // handle
            {.option = "m", .name="mask"}, // mask
            {.option = "p", .name="pending"} // pending
    };

    for(int i = 0;i<4;i++){
        printf("Testy w trubie %s\n", options[i].name);
        SEPARATOR;
        printf("Testy dla fork()\n");
        fflush(stdout);
        for(int sig = 1;sig<31;sig++){
            if(sig == 17)continue;
            EXEC_TEST(sig, ssig, FORK, options[i].option)
        }
        fflush(stdout);
        if(!(equals(options[i].option, "h"))){
            SEPARATOR;
            printf("Testy dla exec()\n");
            fflush(stdout);
            for(int sig = 1;sig<31;sig++){
                if(sig == 17)continue;
                EXEC_TEST(sig, ssig, EXEC, options[i].option)
            }
        }
        SEPARATOR;
        fflush(stdout);
    }
    return 0;
}
