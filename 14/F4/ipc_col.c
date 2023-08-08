#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>
#include<semaphore.h>
#include<gsl/gsl_randist.h>

#define AVAILABLE 1
#define BUSY 0

#define WAITING_TO_PRINT 1
#define PRINTING 2
#define DONE_PRINTING 3



typedef struct student student;
typedef struct group group;
typedef struct staff staff;

struct student {
    int id;
    pthread_t thread;
    sem_t lock;
    int state;
    group* grp;
    int explored;
};

struct group {
    int num, size, print_count;
    student* members;
    sem_t end_print;
    pthread_t group_thread;
};
    
struct staff {
    int id;
    pthread_t thread;
};

const int n_print_station = 4, n_binding_station = 2, n_staff = 2;
int n_students, group_size, n_groups, print_rtu, bind_rtu, rw_rtu, eb_n_sub = 0, eb_rc = 0;
int* printer_state;
time_t start_time;
group** groups;
pthread_mutex_t mutex, *printer_lock, eb_rc_mutex;
sem_t binder, entry_book;
staff* staff_list;
gsl_rng* rng;

void* bind_thread (void* arg);


void* staff_thread (void* arg) {
    int staff_id = *((int*)arg);
    while (eb_n_sub != n_groups) {
        sleep (gsl_ran_poisson (rng, 2.47*staff_id+1.0));
        int read_val;
        pthread_mutex_lock(&eb_rc_mutex);
        eb_rc++;
        if (eb_rc==1) sem_wait (&entry_book);
        pthread_mutex_unlock(&eb_rc_mutex);
        printf("\e[2;35mStaff %d has started reading the entry-book at time %d\n\e[0m",
                staff_id, time(0)-start_time);

        sleep(rw_rtu);
        read_val = eb_n_sub;

        pthread_mutex_lock(&eb_rc_mutex);
        eb_rc--;
        if (eb_rc==0) sem_post (&entry_book);
        pthread_mutex_unlock(&eb_rc_mutex);

        printf("\e[0;35mStaff %d has read the entry-book at time %d; No. of submission = %d\n\e[0m",
                staff_id, time(0)-start_time, read_val);
    };
};

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


void start_print_copy (student* s) {
    // int ps_i = get_printer(s->id);
    pthread_mutex_lock(&mutex);
    s->state = WAITING_TO_PRINT;
    pthread_mutex_unlock(&mutex);
    test_print(s);
    
    //if (s->state == PRINTING) sem_wait(&(s->lock));
};

void end_print_copy (student* s) {
    //printf("About to finish from s%d\n", s->id);
    int ps_i = get_printer(s->id);

    pthread_mutex_lock(&mutex);
    s->state = DONE_PRINTING;
    printf("\e[0;32mStudent %d has finished printing on PS_%d at time %d\n\e[0m", s->id, ps_i+1, time(0)-start_time);
    s->grp->print_count++;
    if (s->grp->print_count == s->grp->size) {
        printf("\e[3;36mGroup %d has finished printing at time %d\n\e[0m", s->grp->num, time(0)-start_time);
        sem_post (&(s->grp->end_print));
    };
    if (s == &(s->grp->members[s->grp->size-1])) {     // check if leader
        pthread_create (&(s->grp->group_thread), NULL, &bind_thread, s->grp);
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

    start_print_copy(s);
    sleep(print_rtu);
    end_print_copy(s);

    return 0;
};


void student_start (student* s) {
    pthread_create (&(s->thread), NULL, &student_thread, s);
};


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

void start_print_copies (group* g) {
    for (int i=0; i<g->size; i++) student_start (&(g->members[i]));
};

void end_print_copies (group* g) {  
    for (int i=0; i<g->size; i++) pthread_join (g->members[i].thread, NULL);
    pthread_join (g->group_thread, NULL);
    for (int i=0; i<g->size; i++) sem_destroy (&(g->members[i].lock));
    sem_destroy (&(g->end_print));
};

void* bind_thread (void* arg) {
    group* g = (group*)arg;

    if (g->print_count != g->size) sem_wait(&(g->end_print));
    printf("Student %d, leader of Group %d is proceeding to bind\n", g->members[g->size-1].id, g->num);

    printf("\e[2;31mGroup %d has started binding at time %d\n\e[0m", g->num, time(0)-start_time);
    sem_wait(&binder);
    sleep(bind_rtu);
    sem_post(&binder);
    printf("\e[1;31mGroup %d has finished binding at time %d\n\e[0m", g->num, time(0)-start_time);

    printf("\e[2;33mStudent %d, leader of Group %d, is writing to the entry-book\n\e[0m", g->members[g->size-1].id, g->num);
    sem_wait(&entry_book);
    sleep(rw_rtu);
    eb_n_sub++;
    sem_post(&entry_book);
    printf("\e[1;33mGroup %d has submitted the report at time %d\n\e[0m", g->num, time(0)-start_time);

    return 0;
};

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
    for (int i=0; i<n_groups; i++) start_print_copies(groups[i]);

    staff_list = (staff*) malloc (n_staff * sizeof(staff));
    for (int i=0; i<n_staff; i++) {
        staff_list[i].id = i+1;
        pthread_create (&(staff_list[i].thread), NULL, &staff_thread, &(staff_list[i].id));
    };
    
    for (int i=0; i<n_groups; i++) end_print_copies (groups[i]);
    for (int i=0; i<n_staff; i++) pthread_join (staff_list[i].thread, NULL);
    for (int i=0; i<n_print_station; i++) pthread_mutex_destroy (&printer_lock[i]);
    pthread_mutex_destroy(&eb_rc_mutex);
    pthread_mutex_destroy(&mutex);
    sem_destroy(&binder);
    printf("\n");
	return 0;
}
