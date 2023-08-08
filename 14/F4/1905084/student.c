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


int get_printer (int s_id) {
    return s_id % n_print_station;
};

void test_print (student* s) {
    int ps_i = get_printer(s->id);
    if (s->state != WAITING_TO_PRINT) return;
    //printf("\e[2;30mStudent %d is waiting to print at PS_%d at time %d\n\e[0m", s->id, ps_i+1, time(0)-start_time);
    if (printer_state[ps_i] == BUSY) sem_wait(&(s->lock));
    //printf("hello from S%d test\n", s->id);

    pthread_mutex_lock(&mutex);
    s->state = PRINTING;
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&printer_lock[ps_i]);
    printer_state[ps_i] = BUSY; 
    pthread_mutex_unlock(&printer_lock[ps_i]);

    sem_post(&(s->lock));
    
    pthread_mutex_lock(&mutex);
    if (!(s->explored))   
        printf("Student %d has arrived at PS_%d at time %d\n", s->id, ps_i+1, time(0)-start_time);
    s->explored = 1;
    pthread_mutex_unlock(&mutex);
};


void end_print_copy (student* s) {
    //printf("About to finish from s%d\n", s->id);
    int ps_i = get_printer(s->id);

    pthread_mutex_lock(&mutex);
    s->state = DONE_PRINTING;
    printf("Student %d has finished printing on PS_%d at time %d\n", s->id, ps_i+1, time(0)-start_time);
    s->grp->print_count++;
    if (s->grp->print_count == s->grp->size) {
        printf("Group %d has finished printing at time %d\n", s->grp->num, time(0)-start_time);
        sem_post (&(s->grp->end_print));
    };
    if (s == &(s->grp->members[s->grp->size-1])) {     // check if leader
        pthread_create (&(s->grp->group_thread), NULL, &bind_and_rw_thread, s->grp);
    };
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&printer_lock[ps_i]);
    printer_state[ps_i] = AVAILABLE;
    pthread_mutex_unlock(&printer_lock[ps_i]);

    for (int i=0; i<s->grp->size; i++) 
        if (s->grp->members[i].id != s->id  && get_printer(s->grp->members[i].id) == ps_i) 
            test_print(&(s->grp->members[i]));

    for (int k=0; k<n_groups; k++) if (groups[k] != s->grp)
        for (int j=0; j<groups[k]->size; j++)
            if (groups[k]->members[j].id != s->id  && get_printer(groups[k]->members[j].id) == ps_i) 
                test_print(&(groups[k]->members[j]));
};

void* student_thread (void* arg) {
    //int id = *((int*)arg);
    student* s = (student*)arg;
    
    int ps_i = get_printer(s->id);
    sleep (gsl_ran_poisson (rng, 1.75)+1);

    test_print(s);
    sleep(print_rtu);
    end_print_copy(s);

    return 0;
};


void student_start (student* s) {
    pthread_create (&(s->thread), NULL, &student_thread, s);
};

