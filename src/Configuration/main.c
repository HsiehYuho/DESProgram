//
//  main.c
//  Configuration
//
//  Created by Hsieh Yuho on 2018/10/10.
//  Copyright © 2018 Yu-Ho Hsieh. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include "build_network.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    if(argc != 5){
        printf("Please pass required arguement with required sequence into program. \n");
        printf("cpssim Time config.txt stats.txt log.txt \n");
        return 1;
    }
    
    double time_limit = atof(argv[1]);
    FILE* config_file = fopen(argv[2], "r");
    FILE* stats_file = fopen(argv[3], "a");
    FILE* log_file = fopen(argv[4], "a");

    if(config_file == NULL || stats_file == NULL || log_file == NULL){
        printf("Read/Write files fails. \n");
        return 1;
    }
    
    bool result = build_network(config_file,stats_file , log_file);
    if(result){
        printf("Build sucess! \n");
        run_sim(time_limit);
    }
    else{
        printf("Build fail! \n");
    }
    
    fclose(config_file);
    fclose(log_file);
    fclose(stats_file);
    printf("Finish!\n");
    
    return 0;
}
