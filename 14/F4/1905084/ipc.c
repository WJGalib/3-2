#include "defs.h"

const int n_print_station = 4, n_binding_station = 2, n_staff = 2;
int n_students, group_size, n_groups, print_rtu, bind_rtu, rw_rtu, eb_n_sub = 0, eb_rc = 0;
int* printer_state;
time_t start_time;
group** groups;
pthread_mutex_t mutex, *printer_lock, eb_rc_mutex;
sem_t binder, entry_book;
staff* staff_list;
gsl_rng* rng;

int main() {	
    scanf("%d%d%d%d%d", &n_students, &group_size, &print_rtu, &bind_rtu, &rw_rtu);
    n_groups = n_students/group_size;
    //printf("%d %d %d %d %d %d", n_students, group_size, print_rtu, bind_rtu, rw_rtu, n_groups);
    start_time = time(0);
    gsl_rng_default_seed = time(0);
    rng = gsl_rng_alloc (gsl_rng_taus);

    printer_state = (int*) malloc (n_print_station * sizeof(int));
    printer_lock = (pthread_mutex_t*) malloc (n_print_station * sizeof(pthread_mutex_t));
    for (int i=0; i<n_print_station; i++) {
        printer_state[i] = AVAILABLE;
        pthread_mutex_init (&printer_lock[i], NULL);
    };

    sem_init (&binder, 0, 2);
    sem_init (&entry_book, 0, 2);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&eb_rc_mutex, NULL);
    
	groups = (group**) malloc (n_groups * sizeof(group*));
    for (int i=0; i<n_groups; i++) groups[i] = create_group(group_size, i+1);
    for (int i=0; i<n_groups; i++) start_group_work (groups[i]);

    staff_list = (staff*) malloc (n_staff * sizeof(staff));
    for (int i=0; i<n_staff; i++) {
        staff_list[i].id = i+1;
        pthread_create (&(staff_list[i].thread), NULL, &staff_thread, &(staff_list[i].id));
    };
    
    for (int i=0; i<n_groups; i++) end_group_work (groups[i]);
    for (int i=0; i<n_staff; i++) pthread_join (staff_list[i].thread, NULL);
    for (int i=0; i<n_print_station; i++) pthread_mutex_destroy (&printer_lock[i]);
    pthread_mutex_destroy(&eb_rc_mutex);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&binder);
    printf("\n");
	return 0;
}
