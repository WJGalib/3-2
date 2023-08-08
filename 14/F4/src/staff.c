#include"defs.h"

extern const int n_print_station, n_binding_station, n_staff;
extern int n_students, group_size, n_groups, print_rtu, bind_rtu, rw_rtu, eb_n_sub, eb_rc;
extern int* printer_state;
extern time_t start_time;
extern group** groups;
extern pthread_mutex_t mutex, *printer_lock, eb_rc_mutex;
extern sem_t binder, entry_book;
extern staff* staff_list;
extern gsl_rng* rng;


void* staff_thread (void* arg) {
    int staff_id = *((int*)arg);
    while (eb_n_sub != n_groups) {
        sleep (gsl_ran_poisson (rng, 2.47*staff_id+1.0));
        int read_val;
        pthread_mutex_lock(&eb_rc_mutex);
        eb_rc++;
        if (eb_rc==1) sem_wait (&entry_book);
        pthread_mutex_unlock(&eb_rc_mutex);
        printf("Staff %d has started reading the entry-book at time %d\n",
                staff_id, time(0)-start_time);

        sleep(rw_rtu);
        read_val = eb_n_sub;

        pthread_mutex_lock(&eb_rc_mutex);
        eb_rc--;
        if (eb_rc==0) sem_post (&entry_book);
        pthread_mutex_unlock(&eb_rc_mutex);

        printf("Staff %d has read the entry-book at time %d; No. of submission = %d\n",
                staff_id, time(0)-start_time, read_val);
    };
};
