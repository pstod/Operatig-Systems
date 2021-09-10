#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<semaphore.h>
#include<math.h>
#include<unistd.h>

int readcount=0,writecount=0;
int data=0;
pthread_mutex_t write_lock;
pthread_mutex_t r_lock;
pthread_mutex_t w_lock;
pthread_mutex_t read_lock;
pthread_mutex_t reader_outer_lock;


void * reader_thread_function(void *no);
void * writer_thread_function(void *no);

void main()
{
	int init_check,r,w,*arg,i;
	
	
	
	printf("\nEnter the number of readers ");
	scanf("%d",&r);
	printf("\nEnter the number of writers ");
	scanf("%d",&w);

	
	
	
	//initialize mutex
	init_check=pthread_mutex_init(&write_lock,NULL);
	if(init_check!=0){printf("\nwrite_lock mutex initialization failed");
	exit(EXIT_FAILURE);}
	
	init_check=pthread_mutex_init(&r_lock,NULL);
	if(init_check!=0){printf("\nr_lock mutex initialization failed");
	exit(EXIT_FAILURE);}
	
	pthread_t reader,writer;
	
	printf("\nCreating reader THREADS ");
	for(i=1;i<=r;i++)
	{
		arg=(int*)malloc(sizeof(int));
		*arg=i;
		printf("\nCreating reader number %d",i);
		init_check=pthread_create(&reader,NULL,reader_thread_function,arg);
		if(init_check!=0){printf("\nreader thread creation failed");
		exit(EXIT_FAILURE);}
	}
	 
	 i=1;
	printf("\n");
	printf("\nCreating writer THREADS");
	for(i=1;i<=w;i++)
	{
		arg=(int*)malloc(sizeof(int));
		*arg=i;
		
		printf("\nCreating writer number %d",i);
		init_check=pthread_create(&writer,NULL,writer_thread_function,arg);
		if(init_check!=0){printf("\nwriter thread creation failed");
		exit(EXIT_FAILURE);}
	}


	pthread_join(reader,NULL);
	pthread_join(writer,NULL);
}

void * reader_thread_function(void *no)

{
	int * num=(int *) no;
	while(1)
	{
	int random_sleep=rand()%10;
	if(random_sleep==0){random_sleep=2;}
	sleep(random_sleep);
	
	//sleep(1);   for less randomness
	
	pthread_mutex_lock(&reader_outer_lock);
	pthread_mutex_lock(&read_lock);
	pthread_mutex_lock(&r_lock);
	readcount++;
	if(readcount==1)
	{
	printf("\n");
	printf("\nReader %d has locked write_lock mutex ",*num);
	pthread_mutex_lock(&write_lock);}
	pthread_mutex_unlock(&r_lock);
	pthread_mutex_unlock(&read_lock);
	pthread_mutex_unlock(&reader_outer_lock);
	
	printf("\nReader %d has started reading",*num);
	printf("\nReader %d has read data %d",*num,data);
	sleep(1); //sleeping to get more readers to come(queue up) and read optional?
	
	
	pthread_mutex_lock(&r_lock);
	readcount--;
	if(readcount==0){pthread_mutex_unlock(&write_lock);
	printf("\nReader %d has unlocked write_lock mutex ",*num);}
	pthread_mutex_unlock(&r_lock);
	
	
	//sleep(1);
	}
	
}

void * writer_thread_function(void *no)
{
	int * num=(int *) no;
	
	while(1)
	{
	int random_sleep=rand()%10;
	if(random_sleep==0){random_sleep=2;}
	sleep(random_sleep);
	//sleep(1);
	
	pthread_mutex_lock(&w_lock);
	writecount++;
	if(writecount==1)
	{
	printf("\n");
	printf("\nwriter %d has locked read_lock mutex preventing readers to queue up ",*num);
	pthread_mutex_lock(&read_lock);}
	pthread_mutex_unlock(&w_lock);
	
	
	pthread_mutex_lock(&write_lock);
	
	
	printf("\nWriter %d has started writing",*num);
	data=data+1;
	printf("\nwriter %d has incremented data to %d",*num,data);
	
	
	
	//a condition can be put to stop threads after certain increment value 
	//and threads will join  (optional?)
	
	pthread_mutex_unlock(&write_lock);
	
	sleep(1); //sleeping to get more writers to come(queue up) and write optional
	
	pthread_mutex_lock(&w_lock);
	writecount--;
	if(writecount==0){pthread_mutex_unlock(&read_lock);
	printf("\nWriter %d has unlocked read_lock mutex ",*num);}
	pthread_mutex_unlock(&w_lock);	
	
	
	
	}

}
