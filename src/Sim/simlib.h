//statistic functions
/*
 * Get the stats waiting time in system per customer
 * @param: the address of desired parameters
 * @return: void
 */
void get_sys_data(double* avg_wait, double* max_wait, double* min_wait);

/*
 * Get the stats waiting time in each queue station
 * @param: the address of desired parameters
 * @return: void
 */
void get_queue_data(int queue_id, double* avg_wait, double* max_wait, double* min_wait);

/*
 * Get the customer count
 * @param: the address of desired parameters
 * @return: void
 */
void get_customer_data_count(int* number_enter_system, int* number_exit_system);

/*
 * Get the customer time staying in system
 * @param: the address of desired parameters
 * @return: void
 */
void get_customer_data_time(double* avg_time_stay_in_system, double* max_time_stay_in_system, double* min_time_stay_in_system);

/*
 * Get the customer average waiting time in system
 * @param: the address of desired parameters
 * @return: void
 */
void get_customer_data_wait(double* avg_time_wait_in_system);

//Simulation API
/*
 * Call to create generator
 * @param: id, P is the average interarrival time, D is the export port
 * @return: int, 0 means success, -1 means fail
 */
int create_generator(int generator_id,double P,int D);

/*
 * Call to create queue
 * @param: id, P is the average serve time, D is the export port
 * @return: int, 0 means success, -1 means fail
 */
int create_queue(int queue_id, double P,int D);


/*
 * Call to create fork
 * @param: id, K is the total nubmer of exports, P is the pointer to an array of probability distributing to different export ports, D is the pointer to an array of export port IDs
 * @return: int, 0 means success, -1 means fail
 */
int create_fork(int fork_id, int K,double* P, int* D);

/*
 * Call to create exit
 * @param: id
 * @return: int, 0 means success, -1 means fail
 */
int create_exit(int exit_id);

/*
 * Call to init the total component in the queue network
 * @param: the component number read from config
 * @return: int, 0 means success, -1 means fail
 */
int init_simulation(int component_num);

/*
 * Call to destroy the components
 * @param: void
 * @return: void
 */
void destroy_simulation(void);

/*
 * Run the simulation
 * @param: time_limit is the time that simulation should shop
 * @return: void
 */
void run_simulation(double end_time);
