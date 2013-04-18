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

pipe_sem_t wrt;
pipe_sem_t read_count_mutex;

int read_count = 0;

void *reader_thread( void *x ) {
	
	//lock
	pipe_sem_wait( &read_count_mutex );
	if( read_count == 0 ) {
		pipe_sem_wait( &wrt );
	}
	read_count++;
	pipe_sem_signal( &read_count_mutex );
	//lock

	//cs
	printf("Reader thread <%ld> enters CS\n", pthread_self());
	sleep(1);
	printf("Reader thread <%ld> is exiting CS\n", pthread_self());
	//cs
	
	//unlock
	pipe_sem_wait( &read_count_mutex );
	read_count--;
	if( read_count == 0 ) {
		pipe_sem_signal( &wrt );
	}
	pipe_sem_signal( &read_count_mutex );
	//unlock
}

void *writer_thread( void *x ) {
	
	//lock
	pipe_sem_wait( &wrt );
	//lock
	
	//cs
	printf("Writer thread <%ld> enters CS\n", pthread_self());
	SHARED_INTEGER = SHARED_INTEGER + 1;
	sleep(1);
	printf("Writer thread <%ld> is exiting CS\n", pthread_self());
	//cs
	
	//unlock
	pipe_sem_signal( &wrt );
	//unlock
}

int main(int argc, char** args) {
	
	perror("be sure to make it a counting semaphore")s
	
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
	pipe_sem_init(&wrt, 1);
	pipe_sem_init(&read_count_mutex, 1);
	
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
	
	sleep(thread_arrival_interval);
	printf("final value:  %d\n", SHARED_INTEGER);
	
	// return 0 to indicate successfull execution
	return 0;
}
