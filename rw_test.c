/**
 * @file   rw_test.c
 * @Author Ryan Kinsey Sanders (rks@iastate.edu)
 * @date   April, 2013
 * @brief  A "writer priority version" of the 
 * reader-writer problem is implemented using
 * the 'pipe_sem' libary.
 *
 * In this file, a " writer priority version" 
 * of the reader-writer problem is implemented
 * using the  'pipe_sem' libary which itself is
 * buit upon unix pipes.  This file futher tests
 * the reader-writer implementation by simulating
 * readers and writers using the pthreads libary
 * and user input from the command line.  One 
 * should refer to the project spects of project
 * 2 for COM S 352 for how to properly input 
 * data and read output of this program.
 */
 
#include <stdio.h>
#include <stdlib.h>

#include "pipe_sem.h"

int SHARED_INTEGER = 0;

pipe_sem_t mutex_1;
pipe_sem_t mutex_2;
pipe_sem_t mutex_3;
pipe_sem_t r;
pipe_sem_t w;

int read_count = 0;
int write_count = 0;

/**
 * @name    reader_thead
 * @brief   A simulation reader thread that will
 * "read" the SHARED_INTEGER.
 *
 * This function will simulate what a reader 
 * thread would do on a shared variable between
 * threads.
 */
void *reader_thread( void *x ) {

	//lock
	pipe_sem_wait(&mutex_3);
		pipe_sem_wait(&r);
			pipe_sem_wait(&mutex_1);
				read_count++;
				if(read_count == 1) {
					pipe_sem_wait(&w);
				}
			pipe_sem_signal(&mutex_1);
		pipe_sem_signal(&r);
	pipe_sem_signal(&mutex_3);
	//lock

	//cs
	printf("Reader thread %ld enters CS\n", pthread_self());
	sleep(3);
	printf("Reader thread %ld is exiting CS\n", pthread_self());
	//cs
	
	//unlock
	pipe_sem_wait(&mutex_1);
		read_count--;
		if(read_count == 0) {
			pipe_sem_signal(&w);
		}
	pipe_sem_signal(&mutex_1);
	//unlock
}

/**
 * @name    writer_thead
 * @brief   A simulation write thread that will
 * "write" to the SHARED_INTEGER.
 *
 * This function will simulate what a writer 
 * thread would do on a shared variable between
 * threads.  In this particular thread the writer
 * thread increments SHARED_INTEGER by 1.
 */
void *writer_thread( void *x ) {
	
	//lock
	pipe_sem_wait(&mutex_2);
	write_count++;
	if(write_count == 1) {
		pipe_sem_wait(&r);
	}
	pipe_sem_signal(&mutex_2);
	
	pipe_sem_wait(&w);
	//lock
	
	//cs
	printf("Writer thread %ld enters CS\n", pthread_self());
	SHARED_INTEGER = SHARED_INTEGER + 1;
	sleep(3);
	printf("Writer thread %ld is exiting CS\n", pthread_self());
	//cs
	
	//unlock
	pipe_sem_signal(&w);
	
	pipe_sem_wait(&mutex_2);
		write_count--;
		if(write_count == 0) {
			pipe_sem_signal(&r);
		}
	pipe_sem_signal(&mutex_2);
	//unlock
	
}

/**
 * @name    main
 * @brief   Start the reader-writer simulation using pipe_sem
 * user libary.
 *
 * This function is called to start the execution of the
 * reader-writer simulation defined in the project 2
 * requrements document posted on the COM S 352 blackboard
 * page.
 *
 * Input Specifications:
 * rw_test <number-of-arriving threads> <a sequence of 0 and 1 separated by a blank-space> <thread arrival interval> 
 * 
 * Exmaple Input String:
 * rw_test 5 0 0 1 1 0 1 
 */
int main(int argc, char** args) {
	
	perror("Counting Semophore?");
	
	// a variable for looping
	int i;

	// number of arriving threads
	int n_arriving_threads = 0;

	// thread arrival interval
	int thread_arrival_interval = 0;
	
	// to be used as a dynamic array of pthread_t structures
	pthread_t *th; 

	// extract number of arriving threads from program arguments
	n_arriving_threads = atoi( args[1] );
	
	// extract thread arrival interval from program arguments
	thread_arrival_interval = atoi(args[n_arriving_threads + 2]);
	
	// allocation dynamic array of pthread_t structures of size n_arriving_threads
	th = (pthread_t*) malloc(sizeof(pthread_t) * n_arriving_threads);

	// init all pipe_sem(s)
	pipe_sem_init(&mutex_1, 1);
	pipe_sem_init(&mutex_2, 1);
	pipe_sem_init(&mutex_3, 1);
	pipe_sem_init(&r, 1);
	pipe_sem_init(&w, 1);
	
	// using the thread_arrival_interval simulate arriving reader/writer threads by calling pthread's pthread_create
	for(i = 0 ; i < n_arriving_threads ; i++) {
		if(atoi (args[i + 2]) == 1) {
			pthread_create(&th[i],NULL,writer_thread,NULL);
		}
		else {
			pthread_create(&th[i],NULL,reader_thread,NULL);
		}
		sleep(thread_arrival_interval);
	}
	
	// join the current thread with all child threads to ensure all child threads finish executing
	for(i = 0 ; i < n_arriving_threads ; i++) {
		pthread_join(th[i], NULL);
	}
	
	// return 0 to indicate successfull execution
	return 0;
}
