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

pipe_sem_t read_count_mutex;
pipe_sem_t write_count_mutex;
pipe_sem_t writer_priority_mutex;
pipe_sem_t reading_mutex;
pipe_sem_t writing_mutex;

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
	pipe_sem_wait(&writer_priority_mutex);
	pipe_sem_wait(&reading_mutex);
	pipe_sem_wait(&read_count_mutex);
	read_count++;
	if(read_count == 1) {
		pipe_sem_wait(&writing_mutex);
	}
	pipe_sem_signal(&read_count_mutex);
	pipe_sem_signal(&reading_mutex);
	pipe_sem_signal(&writer_priority_mutex);
	//lock

	//cs
	printf("Reader thread %ld enters CS\n", pthread_self());
	sleep(3);
	printf("Reader thread %ld is exiting CS\n", pthread_self());
	//cs
	
	//unlock
	pipe_sem_wait(&read_count_mutex);
	read_count--;
	if(read_count == 0) {
		pipe_sem_signal(&writing_mutex);
	}
	pipe_sem_signal(&read_count_mutex);
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
	pipe_sem_wait(&write_count_mutex);
	write_count++;
	if(write_count == 1) {
		pipe_sem_wait(&reading_mutex);
	}
	pipe_sem_signal(&write_count_mutex);
	
	pipe_sem_wait(&writing_mutex);
	//lock
	
	//cs
	printf("Writer thread %ld enters CS\n", pthread_self());
	SHARED_INTEGER = SHARED_INTEGER + 1;
	sleep(3);
	printf("Writer thread %ld is exiting CS\n", pthread_self());
	//cs
	
	//unlock
	pipe_sem_signal(&writing_mutex);
	
	pipe_sem_wait(&write_count_mutex);
	write_count--;
	if(write_count == 0) {
		pipe_sem_signal(&reading_mutex);
	}
	pipe_sem_signal(&write_count_mutex);
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
	pipe_sem_init(&read_count_mutex, 1);
	pipe_sem_init(&write_count_mutex, 1);
	pipe_sem_init(&writer_priority_mutex, 1);
	pipe_sem_init(&reading_mutex, 1);
	pipe_sem_init(&writing_mutex, 1);
	
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
