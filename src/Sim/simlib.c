//
//  simlib.c
//  This simulation library is intended to implement the functions as follow:
//  1. Build and return entities for network.
//  2.Implement the function for each entity.
//  3.Make event Handlers to manage the entities created.
//  4.Maintain variables to collect statistics and create function to expose them.
//  5.Build the simulation engine, executing simulation and destroying simulation.
//
//
//  File written by Cheng Zhang on 10/16/18.
// 
//

#include"simlib.h"
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<stdio.h>

// Simulation clock variable
static double now_time = 0.0;
//data struct
typedef struct Customer{
	double time_create;
	double time_stamp;// managed in library
	double time_wait;
	double time_serve;
    struct Customer* next;
    struct Customer* prev;
}Customer;
typedef struct Component
{
    char type;//'G','Q','F','E'
    char data[0];
}Component;
typedef struct Generator
{
    int ID;//component ID
    double P;//average interarrival time
    int D;//ID of the component to which generated customers are sent
}Generator;
typedef struct Exit{
    int ID;//component ID
}Exit;
typedef struct Queue{
	int ID;//component ID
    double P;//average service time
    int D;//ID of the component to which departing customers are sent
	double total_wtime;
	double min_wtime;
	double max_wtime;
	int total_customer;
	Customer* head;
    Customer* tail;
}Queue;
typedef struct Fork{
    int ID;//component ID
    int K;//number of output ports on the component
    double *P;//P[i] indicating the probability a customer is routed to output port i
    int *D;//D[i] gives the ID of the component connect to output port i
}Fork;
#define C2G(component) ((Generator*)(component->data))
#define C2Q(component) ((Queue*)(component->data))
#define C2F(component) ((Fork*)(component->data))
#define C2E(component) ((Exit*)(component->data))
// Event types
#define	ARRIVAL     1
#define	DEPARTURE   2
//event
typedef struct Event {
	double timestamp;		// event timestamp
    int event_type;         // event type
    int component_id;       // id of component that handles the event
	double serve_time;
	struct Event *next;		// priority queue pointer
}Event;
// Future Event List
static Event fel ={-1.0, -1,-1,0,NULL};
// component list
static Component** component_list;
static int component_size = -1;

//functions declaration
static int is_queue_empty(Queue *q);
static Customer* out_queue(Queue* q);
static void en_queue(Queue* q,Customer* c);
static Event *remove_event (void);
static void schedule_event(Event* e);
static Event* event_router(Customer* c,int component_id);
static void event_handle(Event* e);
static void init_rand(void);
static double urand(void);
static double randexp(double U);
static void clear_fork(Fork* f);
static void clear_queue(Queue *q);
static double generate_customer(Generator* generator,double current_time,Customer** c);
static double simulation_serve(Customer* customer,Queue* queue);
static int simulation_fork(Fork* fork);
static int simulation_exit(Exit* _exit,Customer* customer);

//return NULL if FEL is empty
static Event *remove_event (void)
{
    struct Event *e;
    
    if (fel.next==NULL) return (NULL);
    e = fel.next;		// remove first event in list
    fel.next = e->next;
    return (e);
}
// Schedule new event in FEL
static void schedule_event(Event* e)
{
    Event *p, *q;
 	for (q=&fel, p=fel.next; p!=NULL; p=p->next, q=q->next) {
		if (p->timestamp >= e->timestamp) 
            break;
	}   
    // insert after q (before p)
	e->next = q->next;
	q->next = e;
}
// event router
static Event* event_router(Customer* c,int component_id)
{
    Event *event = NULL;
    switch(component_list[component_id]->type)
    {
        case 'Q':
        {
            Queue *queue = C2Q(component_list[component_id]);
            if(is_queue_empty(queue))
            {
                event = (Event*)malloc(sizeof(Event));
                event->component_id = component_id;
                event->event_type = DEPARTURE;
				event->serve_time = randexp(queue->P);
                event->timestamp = now_time+ event->serve_time;
                event->next = NULL;
            }else
                event = NULL;
            en_queue(queue,c);
        }break;
        case 'F':
        {
            Fork *fork = C2F(component_list[component_id]);
            event = event_router(c,simulation_fork(fork));
        }break;
        case 'E':
        {
            Exit *_exit = C2E(component_list[component_id]);
            simulation_exit(_exit,c);
            event = NULL;
        }break;
    }
    return event;
}
// event handler
static void event_handle(Event* e)
{
    Component* component = component_list[e->component_id];
    switch(e->event_type)
    {
        case ARRIVAL:
        {
            Customer *c;
            double next_time = generate_customer(C2G(component),e->timestamp,&c);
            Event* departure_event = event_router(c,C2G(component)->D);
            if(departure_event)
            {
                schedule_event(departure_event);
            }
            Event *arrival_event = e;//reuse
            e->timestamp = next_time;
            schedule_event(arrival_event);
        }break;
        case DEPARTURE:
        {
            Queue* queue = C2Q(component);
            Customer *c = out_queue(queue);
			c->time_serve = e->serve_time;
            double next_time = simulation_serve(c,queue);
            Event* departure_event = event_router(c,queue->D);
            if(departure_event)
            {
                schedule_event(departure_event);
            }
            if(!is_queue_empty(queue))
            {
                Event* next_departure_event = e;
                next_departure_event->timestamp = next_time;
				next_departure_event->serve_time = next_time - now_time;
                schedule_event(next_departure_event);
            }else
                free(e);
        }break;
        default:
            printf("event type error\n");
            exit(-1);
    }
}
//queue functions
static int is_queue_empty(Queue *q)
{
    if(q->head == q->tail
    &&q->head == NULL)
        return 1;
    else
        return 0;
}
static Customer* out_queue(Queue* q)
{
    Customer *c = NULL;
    if(!is_queue_empty(q))
    {
        c = q->head;
        if(q->head==q->tail)
        {
            q->head=q->tail=NULL;
        }else{
            q->head = q->head->next;
            q->head->prev = NULL;  
        }
		c->next = NULL;
		c->prev = NULL;
    }
    return c;
}
static void en_queue(Queue* q,Customer* c)
{
	c->time_stamp = now_time;
    if(is_queue_empty(q))
    {
        q->tail = q->head = c;
        q->tail->next = NULL;
        q->head->prev = NULL;
    }else{
        c->prev = q->tail;
        q->tail->next = c;
        q->tail = c;
        q->tail->next = NULL;
    }
}
//statistic functions
static int total_customer = 0;//sum of customer exit queue(not exit system)
static double total_wtime = 0;//sum of waiting time
static double max_wtime = -1;
static double min_wtime = -1;
static int enter_customer = 0;
static int exit_customer = 0;
static double total_stay_time = 0;
static double min_stay_time = -1;
static double max_stay_time = -1;
static double total_wait_time = 0;

void get_sys_data(double* avg_wait, double* max_wait, double* min_wait)
{
	*avg_wait = total_wtime / total_customer;
	*max_wait = max_wtime;
	*min_wait = min_wtime;
}
void get_queue_data(int queue_id, double* avg_wait, double* max_wait, double* min_wait)
{
	Queue* queue = C2Q(component_list[queue_id]);
	*avg_wait = queue->total_wtime / queue->total_customer;
	*max_wait = queue->max_wtime;
	*min_wait = queue->min_wtime;
}
void get_customer_data_count(int* number_enter_system, int* number_exit_system)
{
	*number_enter_system = enter_customer;
	*number_exit_system = exit_customer;
}
void get_customer_data_time(double* avg_time_stay_in_system, double* max_time_stay_in_system, double* min_time_stay_in_system)
{
	*avg_time_stay_in_system = total_stay_time / exit_customer;
	*max_time_stay_in_system = max_stay_time;
	*min_time_stay_in_system = min_stay_time;
}
void get_customer_data_wait(double* avg_time_wait_in_system)
{
	*avg_time_wait_in_system = total_wait_time / exit_customer;
}
//random functions
static void init_rand()
{
    srand((unsigned)time(0));
}
static double urand()
{
	double u;
	u = rand() / (double)(RAND_MAX + 1.0);
	return u;
}
static double randexp(double mean)
{
    return -mean*log(1.0-urand());
}
//simulation functions
static void clear_fork(Fork* f)
{
    if(f)
    {
        if(f->P)
            free(f->P);
        if(f->D)
            free(f->D);
    }
}
//free data in queue
static void clear_queue(Queue *q)
{
    while(!is_queue_empty(q))
    {
        Customer *c = out_queue(q);
        if(c)
            free(c);
    }
}
//create customer
static double generate_customer(Generator* generator,double current_time,Customer** c)
{
    double next_time = current_time+randexp(generator->P);
    (*c) = (Customer*)malloc(sizeof(Customer));
    (*c)->next = NULL;
    (*c)->prev = NULL;
    (*c)->time_create = current_time;
    (*c)->time_stamp = 0;
	(*c)->time_wait = 0;
	enter_customer++;
    return next_time;
}
//simulate serving process
static double simulation_serve(Customer* customer,Queue* queue)
{
    double serve_time = randexp(queue->P);
    double next_time = now_time+serve_time;

    total_customer++;
	queue->total_customer++;
	double wtime = now_time - customer->time_stamp - customer->time_serve;
    total_wtime+= wtime;
	customer->time_wait += wtime;
	queue->total_wtime += wtime;
    if(min_wtime==-1||min_wtime>wtime)
        min_wtime = wtime;
    if(max_wtime<wtime)
        max_wtime = wtime;
	if (queue->min_wtime == -1 || queue->min_wtime > wtime)
		queue->min_wtime = wtime;
	if (queue->max_wtime < wtime)
		queue->max_wtime = wtime;
    return next_time;
}
//simulate forking process
static int simulation_fork(Fork* fork)
{
    double p = urand();
    double sump = 0;
    for(int i=0;i<fork->K;i++)
    {
        sump+=fork->P[i];
        if(p<sump)
        {
            return fork->D[i];
        }
    }
	printf("error simulation fork\n");
    return -1;
}
//simulate the exiting process and count some statistics
static int simulation_exit(Exit* _exit,Customer* customer)
{
	exit_customer++;
	total_wait_time += customer->time_wait;
	double stay_time = (now_time - customer->time_create);
	total_stay_time += stay_time;
	if (min_stay_time == -1 | min_stay_time > stay_time)
		min_stay_time = stay_time;
	if (max_stay_time < stay_time)
		max_stay_time = stay_time;
	//free memory allocated in generate_customer() function
    free(customer);
    return 0;
}
//create Generator component
//P is the mean value of generating time
int create_generator(int generator_id,double P,int D)
{
    int ret = 0;
    Component *c = (Component*)malloc(sizeof(Component)+sizeof(Generator));
    c->type = 'G';
    component_list[generator_id] = c;
    if(c)
    {
        Generator *g = C2G(c);
        g->ID = generator_id;
        g->P = P;
        g->D = D;

        //add init event
        Event *e = (Event*)malloc(sizeof(Event));
        e->component_id = g->ID;
        e->event_type = ARRIVAL;
        e->timestamp = 0;
        e->next = NULL;
        schedule_event(e);
    }else
        ret = -1;
    return ret;
}
//create Queue component
//P is the mean value of serving time
int create_queue(int queue_id, double P,int D)
{
    int ret = 0;
    Component *c = (Component*)malloc(sizeof(Component)+sizeof(Queue));
    c->type = 'Q';
    component_list[queue_id] = c;
    if(c)
    {
        Queue* q = C2Q(c);
        q->head = NULL;
        q->tail = NULL;
        q->ID = queue_id;
        q->P = P;
        q->D = D;
		q->total_wtime = 0;
		q->max_wtime = -1;
		q->min_wtime = -1;
		q->total_customer = 0;
    }else
        ret = -1;
    return ret;
}
//create Fork component, 
//K is the number of Fork's out port
//P is the array of possiablity of ports
//D is the component linked by out port
int create_fork(int fork_id, int K,double* P, int* D)
{
    int ret = 0;
    Component *c = (Component*)malloc(sizeof(Component)+sizeof(Fork));
    c->type = 'F';
    component_list[fork_id] = c;
    do
    {
        if(c)
        {
            Fork* f = C2F(c);
            f->ID = fork_id;
            f->K = K;
            f->P = (double*)malloc(sizeof(double)*f->K);
            if(f->P==NULL)
            {
                free(f);
                ret = -1;
                break;
            }
            f->D = (int*)malloc(sizeof(int)*f->K);
            if(f->D==NULL)
            {
                free(f->P);
                free(f);
                ret = -1;
                break;
            }
            for(int i=0;i<K;i++)
            {
                f->P[i] = P[i];
                f->D[i] = D[i];
            }        
        }else
        {
            ret = -1;
            break;
        }
    }while(0);
    return ret;
}
//create Exit component
int create_exit(int exit_id)
{
    int ret = 0;
    Component *c = (Component*)malloc(sizeof(Component)+sizeof(Exit));
    c->type = 'E';
    component_list[exit_id] = c;
    if(c)
    {
        Exit* e = C2E(c);
        e->ID = exit_id;
    }else
        ret = -1;
    return ret;
}
//init simulation, component_num is the total number of component in simulation
int init_simulation(int component_num)
{
    int ret = 0;
    init_rand();
    component_size = component_num;
    component_list = malloc(sizeof(Component*)*component_size);
    if(component_list==NULL)
        ret = -1;
    return ret;
}
//destroy memory allocated in simulation 
void destroy_simulation()
{
    for(int i=0;i<component_size;i++)
    {
        if(component_list[i])
        {
            if(component_list[i]->type=='Q')
            {
                clear_queue(C2Q(component_list[i]));
            }else if(component_list[i]->type=='F')
            {
                clear_fork(C2F(component_list[i]));
            }
            free(component_list[i]);
        }
    }    
    for(Event *e = (&fel)->next;e!=NULL;)
    {
        Event* tmp = e->next;
        free(e);
        e = tmp;
    }
}
void run_simulation(double end_time)
{
    struct Event *e;

    // Main scheduler loop
	while ((e=remove_event()) != NULL) {
		now_time = e->timestamp;
        if (now_time > end_time) break;
		event_handle(e);
	}
}
