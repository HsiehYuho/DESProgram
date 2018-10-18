//statistic functions
void get_sys_data(double* avg_wait, double* max_wait, double* min_wait);
void get_queue_data(int queue_id, double* avg_wait, double* max_wait, double* min_wait);
void get_customer_data_count(int* number_enter_system, int* number_exit_system);
void get_customer_data_time(double* avg_time_stay_in_system, double* max_time_stay_in_system, double* min_time_stay_in_system);
void get_customer_data_wait(double* avg_time_wait_in_system);

//simulation functions
int create_generator(int generator_id,double P,int D);
int create_queue(int queue_id, double P,int D);
int create_fork(int fork_id, int K,double* P, int* D);
int create_exit(int exit_id);
int init_simulation(int component_num);
void destroy_simulation(void);
void run_simulation(double end_time);
