#include <stdio.h>
#include <stdlib.h>

#include "pipe_sem.h"

int main(int argc, char** args) {
	
	// a variable for looping
	int i;

	// number of arriving threads
	int n_arriving_threads = 0;

	// a sequence of 0 and 1 separated by a blank space
	int* sequence = 0;

	// thread arrival interval
	int thread_arrival_interval = 0;

	// extract number of arriving threads from program arguments
	n_arriving_threads = atoi( args[1] );

	// extract sequence from program arguments
	sequence = (int*) malloc(sizeof(int) * n_arriving_threads);
	if(!sequence) exit( -1 );
	for(i = 0 ; i < n_arriving_threads ; i++) {
		sequence[i] = atoi (args[i + 2]);
	}

	// extract thread arrival interval from program arguments
	thread_arrival_interval = atoi(args[n_arriving_threads + 2]);

	pipe_sem_init();
	
	// return 0 to indicate successfull execution
	return 0;
}