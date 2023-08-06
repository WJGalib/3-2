#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>


int count = 0;
pthread_mutex_t m;

void * threadFunc1(void * arg)
{
	int i;
	for(i=1;i<=10000000;i++)
	{
		//printf("%s\n",(char*)arg);
		//sleep(3);
		pthread_mutex_lock(&m);
		count++;
		pthread_mutex_unlock(&m);
	}
}

void * threadFunc2(void * arg)
{
	int i;
	for(i=1;i<=10000000;i++)
	{
		//printf("%s\n",(char*)arg);
		//sleep(1);
		pthread_mutex_lock(&m);
		count--;
		pthread_mutex_unlock(&m);
	}
}





int main(void)
{	
	pthread_t thread1;
	pthread_t thread2;
	pthread_mutex_init(&m, NULL);
	
	char * message1 = "i am thread 1";
	char * message2 = "i am thread 2";	
	
	pthread_create(&thread1,NULL,threadFunc1,(void*)message1 );
	pthread_create(&thread2,NULL,threadFunc2,(void*)message2 );

	//while(1);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_mutex_destroy(&m);
	printf("%d\n", count);
	return 0;
}
