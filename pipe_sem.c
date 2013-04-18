/**
 * @file   pipe_sem.c
 * @Author Ryan Kinsey Sanders (rks@iastate.edu)
 * @date   April, 2013
 * @brief  A counting semaphore libary built upon
 * unnamed pipes.
 *
 * In this file, a C library is implemented (defined
 * in 'pipe_sem.h') .  This libary gives the user 
 * basic functinaly of a semaphore but all busy 
 * waiting is taken out as the implementation is
 * built upon unix unamed pipes.  be developed to
 * implement the semaphore data type based on the
 * unnamed pipe mechanism.
 */

#include <unistd.h> 
#include <sys/ipc.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h>

#include "pipe_sem.h"

#define BUFFER_SIZE 10

/**
 * @name    pipe_sem_wait
 * @brief   Performs a wait operation on the 
 * counting semaphore passed in.
 *
 * This function should be called when the users
 * wishes to acquire an abstract 'lock' on a shared
 * resource.  The user should call pipe_sem_signal 
 * as soon as the users is done reader or writing
 * from the shared resource and called in an order
 * that conforms to the total ordering (defined by
 * the user not this libary) of the user 
 * application to prevent unwanted deadlocks.
 *
 * @param sem  The counting semophore structure
 * that will wait to aquaire the abstract 'lock'.
 */
void pipe_sem_wait( pipe_sem_t *sem ) {
	char buffer[ BUFFER_SIZE ]; 
	read( sem->lock[ 0 ], buffer, BUFFER_SIZE );
}

/**
 * @name    pipe_sem_signal
 * @brief   Performs a signal operation on the 
 * counting semaphore passed in.
 *
 * This function should be called when the users
 * wishes to release an abstract 'lock' on a shared
 * resource.  The user should call pipe_sem_signal 
 * as soon as the users is done reader or writing
 * from the shared resource and called in an order
 * that conforms to the total ordering (defined by
 * the user not this libary) of the user 
 * application to prevent unwanted deadlocks.
 *
 * @param sem  The counting semophore structure
 * that will wait to aquaire the abstract 'lock'.
 */
void pipe_sem_signal( pipe_sem_t *sem ) {
	int pid; 
	pid = fork(); 
	if( pid < 0 ){ 
		perror( "pipe_sem_wait.c :: pipe_sem_signal( pipe_sem_t* sem ) :: Failed to signal semaphore 'sem'.\n" ); 
		exit( 1 ); 
	} 
	if( pid == 0 ){ 
		write( sem->lock[ 1 ], "ok", BUFFER_SIZE );
		exit( 0 ); 
	} 
}

/**
 * @name    pipe_sem_init
 * @brief   Initialize a semaphore and set its 
 * initial value.
 *
 * This function should be called before calling
 * pipe_sem_wait() or pipe_sem_wait().
 *
 * @param sem  The counting semophore structure
 * that will be initalized.
 * @param value The initial value the counting 
 * semophore should be.  If the value is greater
 * than 0 than pipe_sem_signal() is called by 
 * this function.
 */
void pipe_sem_init( pipe_sem_t *sem, int value ) {
	
	// intitalize pipe_sem_t structure
	sem->value = value;
	
	if( pipe(sem->lock) != 0 ){ 
		perror( "pipe_sem_wait.c :: pipe_sem_init( pipe_sem_t* sem, int value ) :: Failed to initialize unnamed pipe.\n" );
		exit( 1 ); 
	}
	if (value > 0) {
		pipe_sem_signal( sem );
	}
}