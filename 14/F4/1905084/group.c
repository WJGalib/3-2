#include "defs.h"

extern const int n_print_station, n_binding_station, n_staff;
extern int n_students, group_size, n_groups, print_rtu, bind_rtu, rw_rtu, eb_n_sub, eb_rc;
extern int* printer_state;
extern time_t start_time;
extern group** groups;
extern pthread_mutex_t mutex, *printer_lock, eb_rc_mutex;
extern sem_t binder, entry_book;
extern staff* staff_list;
extern gsl_rng* rng;


group* create_group (int size, int num) {
    // printf("Creating group %d\n", num);
    group* g = (group*) malloc (sizeof(group));
    student* members = (student*)malloc(size*sizeof(student));
    for (int i=0; i<size; i++) {
        members[i].id = 1+i+size*(num-1);
        sem_init (&(members[i].lock), 0, 0);
        members[i].grp = g;
        members[i].state = WAITING_TO_PRINT;
        members[i].explored = 0;
    };
    g->num = num, g->size = size, g->members = members, g->print_count = 0;
    sem_init (&(g->end_print), 0, 0);
    return g;
};

void start_group_work (group* g) {
    for (int i=0; i<g->size; i++) student_start (&(g->members[i]));
};

void end_group_work (group* g) {  
    for (int i=0; i<g->size; i++) pthread_join (g->members[i].thread, NULL);
    pthread_join (g->group_thread, NULL);
    for (int i=0; i<g->size; i++) sem_destroy (&(g->members[i].lock));
    sem_destroy (&(g->end_print));
};

void* bind_and_rw_thread (void* arg) {
    group* g = (group*)arg;

    if (g->print_count != g->size) sem_wait(&(g->end_print));
    printf("Student %d, leader of Group %d is proceeding to bind\n", g->members[g->size-1].id, g->num);

    printf("Group %d has started binding at time %d\n", g->num, time(0)-start_time);
    sem_wait(&binder);
    sleep(bind_rtu);
    sem_post(&binder);
    printf("Group %d has finished binding at time %d\n", g->num, time(0)-start_time);

    printf("Student %d, leader of Group %d, is writing to the entry-book\n", g->members[g->size-1].id, g->num);
    sem_wait(&entry_book);
    sleep(rw_rtu);
    eb_n_sub++;
    sem_post(&entry_book);
    printf("Group %d has submitted the report at time %d\n", g->num, time(0)-start_time);

    return 0;
};