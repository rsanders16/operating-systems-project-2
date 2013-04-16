#include <unistd.h> 
#include <sys/ipc.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h>

#include "pipe_sem.h"


/* to perform a wait operation on the semaphore */
void pipe_sem_wait( pipe_sem_t *sem ) {
	char buf[10]; 
	read(sem->lock[0],buf,10); 
}

/* to perform a signal operation on the semaphore */
void pipe_sem_signal( pipe_sem_t *sem ) {
	int pid; 
	pid=fork(); 
	if(pid<0){ 
		perror("fail to implement unlock\n"); 
		exit(1); 
	} 
	if(pid==0){ 
		write(sem->lock[1],"ok",10); 
		exit(0); 
	} 
}

/* initialize a semaphore and set its initial value */
void pipe_sem_init( pipe_sem_t *sem, int value ) {
	sem->value = value;
	sem->lock[0] = 0;
	sem->lock[1] = 0;
	if(pipe(sem->lock)!=0){ 
		perror("fail to initialize pipe\n"); 
		exit(1); 
	}
	if (value > 0) {
		pipe_sem_signal(sem);
	}
}