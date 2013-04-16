#define PIPE_SEM_H

struct pipe_sem_t;
typedef struct pipe_sem_t {
	int value;
	int lock[2];
} pipe_sem_t;

#ifndef PIPE_SEM_H

/* initialize a semaphore and set its initial value */
void pipe_sem_init( pipe_sem_t *sem, int value );

/* to perform a wait operation on the semaphore */
void pipe_sem_wait( pipe_sem_t *sem );

/* to perform a signal operation on the semaphore */
void pipe_sem_signal( pipe_sem_t *sem );

#endif
