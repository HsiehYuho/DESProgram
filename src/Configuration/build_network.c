//
//  build_network.c
//  Configuration
//
//  Created by Hsieh Yuho on 2018/10/10.
//  Copyright © 2018 Yu-Ho Hsieh. All rights reserved.
//

#include "build_network.h"

// Read write files
#define BUFFER 256
#define MSG_LENGTH 100

// The limit number for the component
#define UPPER_LIMIT 100
#define LOWER_LIMIT 1

// Constraint of component line
#define TYPE_LENGTH 1
#define GENERATOR 'G'
#define QUEUE 'Q' // in queue station
#define FORK 'F'
#define EXIT 'X'

typedef struct CMP{
    char type;
    char id;
    double time;
    int export;
    int exports_num;
    int* exports;
    double* probs;
} CMP;

/* Helper functions */
bool is_positive_double(char* token);
bool is_postive_int(char* token);
void print_cmp_arr(CMP** cmp_arr, int num_of_cmp);
void log_msg(char* message);
void free_cmps(void);
bool is_connection_valid(CMP** cmp_arr, int cmp_num);
void get_stats(void);
void log_stats(char* message);

/*Global variable*/
FILE* log_file = NULL;
FILE* stats_file = NULL;
CMP** cmp_arr = NULL;
int cmp_num = 0;

bool build_network(FILE* config_file, FILE* _stats, FILE* _log){
    // Read file from file path
    // Check the parameters for different types of component are correct
    // Check the circuit is correct
    // Log the error
    
    // Log date
    log_file = _log;
    stats_file = _stats;
    
    char date_buff[20];
    struct tm *sTm;
    
    time_t now = time(0);
    sTm = gmtime (&now);
    strftime (date_buff, sizeof(date_buff), "%Y-%m-%d %H:%M:%S", sTm);
    log_msg(date_buff);
    log_stats(date_buff);
    
    // Read first line of file
    char buffer[BUFFER];
    if(!fgets(buffer, BUFFER, config_file)){
        log_msg("Read failed");
        return false;
    };
    
    cmp_num = atoi(buffer);
    if(!is_postive_int(buffer) || UPPER_LIMIT < cmp_num || LOWER_LIMIT > cmp_num){
        log_msg("Invalid input component num");
        return false;
    }
    
    // Create cmp array to test connection and id_set to test duplicate id
    bool ID_set[cmp_num];
    cmp_arr = malloc(cmp_num * sizeof(CMP*));
    for(int i = 0; i < cmp_num; i++){
        ID_set[i] = false;
        cmp_arr[i] = malloc(sizeof(CMP));
        cmp_arr[i]->type = ' ';
        cmp_arr[i]->id = -1;
        cmp_arr[i]->time = -1;
        cmp_arr[i]->export = -1;
        cmp_arr[i]->probs = NULL;
        cmp_arr[i]->exports = NULL;
    }

    // Read file line by line
    for(int i = 0; i < cmp_num; i++){
        if(!fgets(buffer, BUFFER, config_file)){
            log_msg("Num of rows does not match to total number of component");
            return false;
        }

        char* token = NULL;
        
        // For log file, msg contains ith row
        char msg[MSG_LENGTH];
        sprintf(msg, "%d row's ", i);
        
        // ID
        token = strtok(buffer," \n");
        int id = atoi(token);
        if(!is_postive_int(token) || id >= cmp_num || id < 0 || ID_set[id]){
            strcat( msg, "id is invalid" );
            log_msg(msg);
            return false;
        }
        ID_set[id] = true;
        
        // Type
        token = strtok (NULL, " \n");
        size_t len = strlen(token);
        if(len != TYPE_LENGTH){
            strcat( msg, "type should be 1 character" );
            log_msg(msg);
            return false;
        }
        char type = (*token);
        
        if(type != GENERATOR && type != QUEUE && type != FORK && type != EXIT){
            strcat( msg, "type is unknown" );
            log_msg(msg);
            return false;
        }
        
        if(type == GENERATOR || type == QUEUE){
            // Serve time or generation time
            token = strtok (NULL, " \n");
            if(!is_positive_double(token)){
                strcat( msg, "generator or queue time should be positive double" );
                log_msg(msg);
                return false;
            }
            double interval_time = atof(token);

            // Export id
            token = strtok (NULL, " \n");
            if(token == NULL){
                strcat( msg, "does not have export id");
                log_msg(msg);
                return false;
            }
            int export = atoi(token);
            if(!is_postive_int(token) || export >= cmp_num){
                strcat( msg, "export does not exist" );
                log_msg(msg);
                return false;
            }
            
            // Should not have extra parameters
            token = strtok (NULL, " \n");
            if(token != NULL){
                strcat( msg, "has too many parameters");
                log_msg(msg);
                return false;
            }
            
            
            // Update cmp_arr
            CMP* cmp = cmp_arr[id];
            cmp->id = id;
            cmp->type = type;
            cmp->time = interval_time;
            cmp->export = export;
        }
        
        if(type == FORK){
            // Number of exports
            token = strtok (NULL, " \n");
            if(!is_postive_int(token)){
                strcat( msg, "export is not valid" );
                log_msg(msg);
                return false;
            }

            int exports_num = atoi(token);
            double probs[exports_num];
            int exports[exports_num];
            
            // Check probs
            for(int i = 0; i < exports_num; i++){
                token = strtok (NULL, " \n");
                if(!is_positive_double(token)){
                    strcat( msg, "probabilities should be positive double" );
                    log_msg(msg);
                    return false;
                }
                probs[i] = atof(token);
            }
            double sum = 0.0;
            for(int i = 0; i < exports_num; i++){
                sum += probs[i];
            }
            if(sum != 1.0){
                strcat( msg, "sum of probabilities should be 1" );
                log_msg(msg);
                return false;
            }
            
            // Check exports
            for(int i = 0; i < exports_num; i++){
                token = strtok (NULL, " \n");
                if(!is_postive_int(token) || atoi(token) >= cmp_num){
                    strcat( msg, "export is invalid" );
                    log_msg(msg);

                    return false;
                }
                int export = atoi(token);
                exports[i] = export;
            }
            
            // Check ends
            token = strtok (NULL, " \n");
            if(token != NULL){
                strcat( msg, "has too many parameters" );
                log_msg(msg);
                return false;
            }
            
            // Update cmp_arr
            CMP* cmp = cmp_arr[id];
            cmp->id = id;
            cmp->type = type;
            cmp->exports_num = exports_num;
            
            // Copy an array, because the exports and probs will be collected later
            cmp->probs = malloc(sizeof(double) * exports_num);
            cmp->exports = malloc(sizeof(int) * exports_num);
            for(int i = 0; i < exports_num; i++){
                cmp->probs[i] = probs[i];
                cmp->exports[i] = exports[i];
            }
            printf("");
            
        }
        
        if(type == EXIT){
            token = strtok (NULL, " \n");
            if(token != NULL){
                strcat( msg, "has too many parameters");
                log_msg(msg);
                return false;
            }

            // Update cmp_arr
            CMP* cmp = cmp_arr[id];
            cmp->id = id;
            cmp->type = type;
        }
    }
    
    // Should not have remaining lines
    if(fgets(buffer, BUFFER, config_file)){
        log_msg("Num of rows does not match to total number of component");
        return false;
    }
    printf("Valid config file. \n");
    
    // Test the connection of components
    if(!is_connection_valid(cmp_arr,cmp_num)){
        printf("Invalid component connection. \n");
        return false;
    }
    else{
        printf("Valid component connection. \n");

    }
    
    return true;
};

bool is_connection_valid(CMP** cmp_arr, int cmp_num){
    for(int i = 0; i < cmp_num; i++){
        CMP* cmp = cmp_arr[i];
        char type = cmp->type;
        
        char msg[MSG_LENGTH];
        sprintf(msg, "ID: %d cmp ", i);
        
        switch(type) {
            case GENERATOR :
            {
                int export = cmp->export;
                if(cmp_arr[export]->type == GENERATOR || cmp_arr[export]->type == EXIT){
                    strcat( msg, "export cannot be generator or exit" );
                    log_msg(msg);
                    return false;
                }
            }break;
                
            case QUEUE:
            {
                int export = cmp->export;
                if(cmp_arr[export]->type == GENERATOR){
                    strcat( msg, "export cannot be generator" );
                    log_msg(msg);
                    return false;
                }
            }break;
                
            case FORK:
            {
                int* exports = cmp->exports;
                int exports_num = cmp->exports_num;
                for(int j = 0; j < exports_num; j++){
                    int export = exports[j];
                    if(cmp_arr[export]->type == GENERATOR){
                        strcat( msg, "export cannot be generator" );
                        log_msg(msg);
                        return false;
                    }
                }
            }break;
            case EXIT:
            {
            }break;
        }
    }
    log_msg("Config Valid");
    return true;
}

void run_sim(double time_limit){

    // Build network
    init_simulation(cmp_num);

    for(int i = 0; i < cmp_num; i++){
        CMP* cmp = cmp_arr[i];
        char type = cmp->type;
        switch(type) {
            case GENERATOR :
            {
                create_generator(i, cmp->time, cmp->export);
            }break;
            case QUEUE :
            {
                create_queue(i, cmp->time, cmp->export);
            }break;
            case FORK :
            {
                create_fork(i, cmp->exports_num, cmp->probs, cmp->exports);
            }break;
            case EXIT :
            {
                create_exit(i);
            }break;
        }
    }
    
    // Run simulation
    run_simulation(time_limit);
    
    // After finish simulation, get stats
    get_stats();
    
}

void get_stats(){
    char stats[MSG_LENGTH];
    
    log_stats("Stats: ");
    
    // System data
    double avg_stays_in_sys, max_stays_in_sys, min_stays_in_sys;
    get_customer_data_time(&avg_stays_in_sys, &max_stays_in_sys, &min_stays_in_sys);
    sprintf(stats, "Avg: %.2f, Max: %.2f, Min: %.2f", avg_stays_in_sys, max_stays_in_sys, min_stays_in_sys);
    
    log_stats("The time that customer stays in system:");
    log_stats(stats);

    double avg_waits_in_sys;
    get_customer_data_wait(&avg_waits_in_sys);
    sprintf(stats, "Avg: %.2f,", avg_waits_in_sys);
    log_stats("The time that customer waits in system:");
    log_stats(stats);
    
    double sys_avg_wait, sys_max_wait, sys_min_wait;
    get_sys_data(&sys_avg_wait, &sys_max_wait, &sys_min_wait);
    log_stats("The waiting time of whole system:");
    sprintf(stats, "Avg: %.2f, Max: %.2f, Min %.2f", sys_avg_wait, sys_max_wait, sys_min_wait);
    log_stats(stats);
    
    // Customer data
    int customer_in_sys, customer_out_sys;
    get_customer_data_count(&customer_in_sys, &customer_out_sys);
    log_stats("The customer count for system:");
    sprintf(stats, "In: %d, Out: %d", customer_in_sys, customer_out_sys);
    log_stats(stats);

    // Queue data
    for(int i = 0; i < cmp_num; i++){
        CMP* cmp = cmp_arr[i];
        double avg_wait_in_q, max_wait_in_q, min_wait_in_q;
        if(cmp->type == QUEUE){
            sprintf(stats, "Queue%d waiting stats", i);
            log_stats(stats);
            get_queue_data(i, &avg_wait_in_q, &max_wait_in_q, &min_wait_in_q);
            sprintf(stats, "Avg: %.2f, Max: %.2f, Min: %2.f.", avg_wait_in_q, max_wait_in_q, min_wait_in_q);
            log_stats(stats);
        }
    }
    log_stats("\n");
    
    free_cmps();
}

// Helper functions
bool is_positive_double(char* token){
    if(token == NULL){
        return false;
    }
    char *endptr;
    errno = 0;
    double result = strtod(token, &endptr);
    if (errno != 0 || *endptr != '\0' || result < 0) {
        return false;
    }
    else{
        return true;
    }
}

bool is_postive_int(char* token){
    if(token == NULL){
        return false;
    }
    int int_num = atoi(token);
    double double_num = atof(token);
    
    if((double)int_num != double_num){
        return false;
    }
    if(int_num < 0){
        return false;
    }
    return true;
}

void print_cmp_arr(){
    for(int i = 0; i < cmp_num; i++){
        CMP* cmp = cmp_arr[i];
        if(cmp->type == GENERATOR || cmp->type == QUEUE){
            printf("ID: %d, Type: %c, Time: %.2f, Export: %d \n", i,cmp->type,cmp->time,cmp->export);
        }
        if(cmp->type == FORK){
            printf("ID: %d, Type: %c \n", i,cmp->type);
            for(int j = 0; j < cmp->exports_num; j++){
                int export = cmp->exports[j];
                double prob = cmp->probs[j];
                printf("        %d, %.2f \n", export, prob);
            }
        }
        if(cmp->type == EXIT){
            printf("ID: %d, Type: %c \n", i,cmp->type);
        }
    }
}

void log_msg(char* message){
    fprintf(log_file, "%s \n", message);
    return;
}

void log_stats(char* message){
    fprintf(stats_file, "%s \n", message);
    return;
}

void log_date(FILE* file){
    
}

void free_cmps(){
    for(int i = 0; i < cmp_num; i++){
        CMP* cmp = cmp_arr[i];
        if(cmp->type == FORK){
            free(cmp->exports);
            free(cmp->probs);
        }
        free(cmp_arr[i]);
    }
}
