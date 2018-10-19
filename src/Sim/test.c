//
//  test.c
//  This test file is used to generate a self-made queueing network to 
//  test whether all the functions in simlib.c can work normally.
//
//
//  File written by Cheng Zhang on 10/16/18.
// 
//

#include"simlib.h"
#include<stdio.h>
int main()
{
	init_simulation(18);//component number
	
	create_generator(0,15,1);//0 G 15.0 1
	create_queue(1,0.2,2);//1 Q 20.0 2
	double p1[] = { 0.1 ,0.15 ,0.2, 0.25 ,0.3 };
	int d1[] = { 3,4,5,6,7 };
	create_fork(2, 5, p1, d1);//2 F 4 0.3 0.2 0.4 0.1 3 4 5 1
	create_queue(3, 3.0, 8);//3 Q 7.0 1
	create_queue(4, 2.5, 9);//4 Q 5.0 1
	create_queue(5, 3.3, 10);//4 Q 5.0 1
	create_queue(6, 2.8, 11);//4 Q 5.0 1
	create_queue(7, 1.5, 12);//4 Q 5.0 1
	double p2[] = { 0.5 ,0.5};
	int d2[] = { 13,14 };
	create_fork(8, 2, p2, d2);
	create_fork(9, 2, p2, d2);
	create_fork(10, 2, p2, d2);
	create_fork(11, 2, p2, d2);
	create_fork(12, 2, p2, d2);
	create_queue(13, 1.0, 15);
	create_queue(14, 1.0, 16);
	double p3[] = { 0.95 ,0.05 };
	int d3[] = { 17,2 };
	create_fork(15, 2, p3, d3);
	create_fork(16, 2, p3, d3);
	create_exit(17);//5 X

	run_simulation(1000.0);

	//get statisitcs
	double avg_wait, max_wait, min_wait;
	get_sys_data(&avg_wait, &max_wait, &min_wait);

	double q3_avg_wait, q3_max_wait, q3_min_wait;
	get_queue_data(3, &q3_avg_wait, &q3_max_wait, &q3_min_wait);
	double q4_avg_wait, q4_max_wait, q4_min_wait;
	get_queue_data(4, &q4_avg_wait, &q4_max_wait, &q4_min_wait);
	double q5_avg_wait, q5_max_wait, q5_min_wait;
	get_queue_data(5, &q5_avg_wait, &q5_max_wait, &q5_min_wait);
	double q6_avg_wait, q6_max_wait, q6_min_wait;
	get_queue_data(6, &q6_avg_wait, &q6_max_wait, &q6_min_wait);
	double q7_avg_wait, q7_max_wait, q7_min_wait;
	get_queue_data(7, &q7_avg_wait, &q7_max_wait, &q7_min_wait);
	double q13_avg_wait, q13_max_wait, q13_min_wait;
	get_queue_data(13, &q13_avg_wait, &q13_max_wait, &q13_min_wait);
	double q14_avg_wait, q14_max_wait, q14_min_wait;
	get_queue_data(14, &q14_avg_wait, &q14_max_wait, &q14_min_wait);

	int number_enter_system, number_exit_system;
	get_customer_data_count(&number_enter_system, &number_exit_system);

	double avg_time_stay_in_system, max_time_stay_in_system, min_time_stay_in_system;
	get_customer_data_time(&avg_time_stay_in_system, &max_time_stay_in_system, &min_time_stay_in_system);

	double avg_time_wait_in_system;
	get_customer_data_wait(&avg_time_wait_in_system);

	//destroy simulation ,pay attention to the usage order
	destroy_simulation();

	printf("avg_wait: %.2lf ,max_wait: %.2lf ,min_wait: %.2lf\n", avg_wait, max_wait,min_wait);
	printf("queue_id: 3 ,avg_wait: %.2lf ,max_wait: %.2lf ,min_wait: %.2lf\n", q3_avg_wait, q3_max_wait, q3_min_wait);
	printf("queue_id: 4 ,avg_wait: %.2lf ,max_wait: %.2lf ,min_wait: %.2lf\n", q4_avg_wait, q4_max_wait, q4_min_wait);
	printf("queue_id: 5 ,avg_wait: %.2lf ,max_wait: %.2lf ,min_wait: %.2lf\n", q5_avg_wait, q5_max_wait, q5_min_wait);
	printf("queue_id: 6 ,avg_wait: %.2lf ,max_wait: %.2lf ,min_wait: %.2lf\n", q6_avg_wait, q6_max_wait, q6_min_wait);
	printf("queue_id: 7 ,avg_wait: %.2lf ,max_wait: %.2lf ,min_wait: %.2lf\n", q7_avg_wait, q7_max_wait, q7_min_wait);
	printf("queue_id: 13 ,avg_wait: %.2lf ,max_wait: %.2lf ,min_wait: %.2lf\n", q13_avg_wait, q13_max_wait, q13_min_wait);
	printf("queue_id: 14 ,avg_wait: %.2lf ,max_wait: %.2lf ,min_wait: %.2lf\n", q14_avg_wait, q14_max_wait, q14_min_wait);
	printf("number_enter_system: %d ,number_exit_system: %d\n", number_enter_system, number_exit_system);
	printf("avg_time_stay_in_system: %.2lf ,max_time_stay_in_system: %.2lf ,min_time_stay_in_system: %.2lf\n", avg_time_stay_in_system, max_time_stay_in_system, min_time_stay_in_system);
	printf("avg_time_wait_in_system: %.2lf\n", avg_time_wait_in_system);
    return 0;
}