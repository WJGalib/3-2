#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>
#include<semaphore.h>


#define AVAILABLE 1
#define BUSY 0

#define IDLE 0
#define WAITING_TO_PRINT 1
#define PRINTING 2
#define DONE_PRINTING 3

typedef struct student student;
typedef struct group group;

struct student {
    int id;
    pthread_t thread;
    sem_t lock;
    int state;
    group* grp;
    int explored;
};

struct group {
    int num, size;
    student* members;
};
    

const int n_print_station = 4, n_binding_station = 2;
int n_students, group_size, n_groups, print_rtu, bind_rtu, rw_rtu;
int* printer_state;
time_t start_time;
group** groups;
pthread_mutex_t mutex, *printer_lock;

int get_printer (int s_id) {
    return s_id % n_print_station;
};

void test_print (student* s) {
    int ps_i = get_printer(s->id);
    if (s->state == IDLE) {
        pthread_mutex_lock(&mutex);
        s->state = WAITING_TO_PRINT;
        pthread_mutex_unlock(&mutex);
    }
    if (s->state != WAITING_TO_PRINT) return;
    printf("Student %d is waiting to print at PS_%d at time %d\n", s->id, ps_i+1, time(0)-start_time);
    if (printer_state[ps_i] == BUSY) sem_wait(&(s->lock));
    //printf("hello from S%d test\n", s->id);

    pthread_mutex_lock(&mutex);
    s->state = PRINTING;
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&printer_lock[ps_i]);
    printer_state[ps_i] = BUSY; 
    pthread_mutex_unlock(&printer_lock[ps_i]);

    sem_post(&(s->lock));
    
    printf("Student %d has arrived at PS_%d at time %d\n", s->id, ps_i+1, time(0)-start_time);
};


void start_print_copy (student* s) {
    // int ps_i = get_printer(s->id);
    // pthread_mutex_lock(&mutex);
    // s->state = WAITING_TO_PRINT;
    // pthread_mutex_unlock(&mutex);
    test_print(s);
    
    //if (s->state == PRINTING) sem_wait(&(s->lock));
};

void end_print_copy (student* s) {
    //printf("About to finish from s%d\n", s->id);
    int ps_i = get_printer(s->id);

    pthread_mutex_lock(&mutex);
    s->state = DONE_PRINTING;
    pthread_mutex_unlock(&mutex);

    printf("\033[0;32mStudent %d has finished printing on PS_%d at time %d\n\033[0m", s->id, ps_i+1, time(0)-start_time);

    pthread_mutex_lock(&printer_lock[ps_i]);
    printer_state[ps_i] = AVAILABLE;
    pthread_mutex_unlock(&printer_lock[ps_i]);

    int assigned = 0;
    for (int i=0; i<s->grp->size; i++) {
        if (s->grp->members[i].id != s->id  && get_printer(s->grp->members[i].id) == ps_i) {
            if (s->grp->members[i].state == IDLE) {
                test_print(&(s->grp->members[i]));
            } else {
                int semval;
                sem_getvalue (&(s->grp->members[i].lock), &semval);
                if (semval == 0) sem_post(&(s->grp->members[i].lock));
                assigned = 1;
                break;
            };
        };
    };

    if (assigned) for (int k=0; k<n_groups; k++) if (groups[k] != s->grp) {
        int break_outer = 0;
        for (int j=0; j<groups[k]->size; j++) {
            if (groups[k]->members[j].id != s->id  && get_printer(groups[k]->members[j].id) == ps_i) {
                if (groups[k]->members[j].state == IDLE) {
                    test_print(&(groups[k]->members[j]));
                } else {
                    int semval;
                    sem_getvalue (&(groups[k]->members[j].lock), &semval);
                    if (semval == 0) sem_post(&(groups[k]->members[j].lock));
                    break_outer = 1;
                    break;
                };
            };
        };
        if (break_outer) break;
    };
};

void* print_copy_thread (void* arg) {
    //int id = *((int*)arg);
    student* s = (student*)arg;
    int ps_i = get_printer(s->id);
    //sleep(rand()%3+2);

    start_print_copy(s);
    sleep(print_rtu);
    end_print_copy(s);

    return 0;
};


void print_copy (student* s) {
    pthread_create (&(s->thread), NULL, &print_copy_thread, s);
};


group* create_group (int size, int num) {
    // printf("Creating group %d\n", num);
    group* g = (group*)malloc(sizeof(group));
    student* members = (student*)malloc(size*sizeof(student));
    for (int i=0; i<size; i++) {
        members[i].id = 1+i+size*(num-1);
        sem_init (&(members[i].lock), 0, 0);
        members[i].grp = g;
        members[i].state = IDLE;
    };
    g->num = num, g->size = size, g->members = members;
    return g;
};

void start_print_copies (group* g) {
    for (int i=0; i<g->size; i++) print_copy(&(g->members[i]));
};

void end_print_copies (group* g) {
    for (int i=0; i<g->size; i++) pthread_join(g->members[i].thread, NULL);
    printf("\033[0;36mGroup %d has finished printing at time %d\n\033[0m", g->num, time(0)-start_time);
    for (int i=0; i<g->size; i++) sem_destroy(&(g->members[i].lock));
};


int main() {	
    scanf("%d%d%d%d%d", &n_students, &group_size, &print_rtu, &bind_rtu, &rw_rtu);
    n_groups = n_students/group_size;
    //printf("%d %d %d %d %d %d", n_students, group_size, print_rtu, bind_rtu, rw_rtu, n_groups);
    start_time = time(0);
    srand(start_time); 
    printer_state = (int*) malloc (n_print_station * sizeof(int));
    printer_lock = (pthread_mutex_t*) malloc (n_print_station * sizeof(pthread_mutex_t));
    for (int i=0; i<n_print_station; i++) {
        printer_state[i] = AVAILABLE;
        pthread_mutex_init (&printer_lock[i], NULL);
    };
	// for (int i=0; i<n_print_station; i++) pthread_mutex_init(&printer_state[i], NULL);
        // printf("hello\n");
    pthread_mutex_init(&mutex, NULL);
	groups = (group**)malloc(n_groups * sizeof(group*));
    for (int i=0; i<n_groups; i++) groups[i] = create_group(group_size, i+1);
    for (int i=0; i<n_groups; i++) start_print_copies(groups[i]);
    for (int i=0; i<n_groups; i++) end_print_copies(groups[i]);

	// for (int i=0; i<n_print_station; i++) pthread_mutex_destroy(&printer_state[i]);
    for (int i=0; i<n_print_station; i++) pthread_mutex_destroy (&printer_lock[i]);
    pthread_mutex_destroy(&mutex);
	return 0;
}
