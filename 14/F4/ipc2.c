#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>

const int n_print_station = 4, n_binding_station = 2;
int n_students, group_size, n_groups, print_rtu, bind_rtu, rw_rtu;
pthread_mutex_t* printers;
time_t start_time;

typedef struct {
    int id;
    pthread_t thread;
} student;


void start_print (int id) {
    int ps_i = id%n_print_station;
    pthread_mutex_lock (&printers[ps_i]);
    printf("Studnent %d has arrived at print station %d at time %d\n", id, ps_i+1, time(0)-start_time);
}

void finish_print(int id) {
    int ps_i = id%n_print_station;
    sleep(print_rtu);
    pthread_mutex_unlock (&printers[ps_i]);
    printf("Studnent %d has finished printing on PS_%d at time %d\n", id, ps_i+1, time(0)-start_time);
}

void* print_copy_thread (void* arg) {
    int id = *((int*)arg);
    int ps_i = id%n_print_station;
    sleep(rand()%3+2);
    start_print(id);
    finish_print(id);
    return 0;
};

void print_copy (student* s) {
    pthread_create (&(s->thread), NULL, &print_copy_thread, &(s->id));
};


typedef struct {
    int num, size;
    student* members;
} group;
    

group* create_group (int size, int num) {
    // printf("Creating group %d\n", num);
    group* g = (group*)malloc(sizeof(group));
    student* members = (student*)malloc(size*sizeof(student));
    for (int i=0; i<size; i++) members[i].id = 1+i+size*(num-1);
    g->num = num, g->size = size, g->members = members;
    return g;
};

void start_print_copies (group* g) {
    for (int i=0; i<g->size; i++) print_copy(&(g->members[i]));
};

void end_print_copies (group* g) {
    for (int i=0; i<g->size; i++) pthread_join(g->members[i].thread, NULL);
    printf("Group %d has finished printing at time %d\n", g->num, time(0)-start_time);
};


int main() {	
    scanf("%d%d%d%d%d", &n_students, &group_size, &print_rtu, &bind_rtu, &rw_rtu);
    n_groups = n_students/group_size;
    // printf("%d %d %d %d %d %d", n_students, group_size, print_rtu, bind_rtu, rw_rtu, n_groups);
    start_time = time(0);
    srand(start_time); 
    printers = (pthread_mutex_t*)malloc (n_print_station * sizeof(pthread_mutex_t));
	for (int i=0; i<n_print_station; i++) pthread_mutex_init(&printers[i], NULL);
        // printf("hello\n");
	group** groups = (group**)malloc(n_groups * sizeof(group*));
    for (int i=0; i<n_groups; i++) groups[i] = create_group(group_size, i+1);
    for (int i=0; i<n_groups; i++) start_print_copies(groups[i]);
    for (int i=0; i<n_groups; i++) end_print_copies(groups[i]);


	for (int i=0; i<n_print_station; i++) pthread_mutex_destroy(&printers[i]);
	return 0;
}
