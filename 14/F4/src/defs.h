#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>
#include<gsl/gsl_randist.h>


#define AVAILABLE 1
#define BUSY 0

#define WAITING_TO_PRINT 0
#define PRINTING 1
#define DONE_PRINTING 2


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

int get_printer (int s_id);
void test_print (student* s);
void end_print_copy (student* s);
void* student_thread (void* arg);
void student_start (student* s);



struct group {
    int num, size, print_count;
    student* members;
    sem_t end_print;
    pthread_t group_thread;
};
    
group* create_group (int size, int num);
void* bind_thread (void* arg);
void start_group_work (group* g);
void end_group_work (group* g);
void* bind_and_rw_thread (void* arg);



struct staff {
    int id;
    pthread_t thread;
};

void* staff_thread (void* arg) ;


