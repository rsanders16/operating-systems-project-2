/**
 * @file   pipe_sem.h
 * @Author Ryan Kinsey Sanders (rks@iastate.edu)
 * @date   April, 2013
 * @brief  A counting semaphore libary built upon
 * unnamed pipes.
 *
 * In this file, a C library is defined (implemented
 * in 'pipe_sem.c') .  This libary gives the user 
 * basic functinaly of a semaphore but all busy 
 * waiting is taken out as the implementation is
 * built upon unix unamed pipes.  be developed to
 * implement the semaphore data type based on the
 * unnamed pipe mechanism.
 */
 
#define PIPE_SEM_H

/**
 * @name    pipe_sem_t
 * @brief   A counting semaphore structure that 
 * will be based on unix unnamed pipes.
 *
 * Each pipe_sem_t structure will have a value 
 * that represents the current value of the 
 * counting semaphore and an integer array that
 * will be used with the read() and write() 
 * function of the pipe libarys of unix.
 */
struct pipe_sem_t;
typedef struct pipe_sem_t {
	int value;
	int lock[2];
} pipe_sem_t;

#ifndef PIPE_SEM_H

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
void pipe_sem_init( pipe_sem_t *sem, int value );

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
void pipe_sem_wait( pipe_sem_t *sem );

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
void pipe_sem_signal( pipe_sem_t *sem );

#endif
