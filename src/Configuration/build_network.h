//
//  build_network.h
//  Configuration
//
//  Created by Hsieh Yuho on 2018/10/10.
//  Copyright © 2018 Yu-Ho Hsieh. All rights reserved.
//

#ifndef build_network_h
#define build_network_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include "../Sim/simlib.h"

#endif /* build_network_h */

bool build_network(FILE* config_file, FILE* _stats_file, FILE* log_file);
void run_sim(double time_limit);

