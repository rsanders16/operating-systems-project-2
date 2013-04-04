#include <unistd.h> 
#include <sys/ipc.h> 
#include <stdlib.h> 
#include <string.h> 
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

#include "pipe_sem.h"

/* called when releasing a lock */ 
void lock_release (int fd[2]);

/* called when acquiring a lock */
void lock_acquire(int fd[2]);

/* called when initializing a lock */
void lock_init(int fd[2]);

/* used to implement an unamed pipe */
int lock[2];

/* initialize a semaphore and set its initial value */
void pipe_sem_init( pipe_sem_t *sem, int value ) {
	sem->value = value; 
	lock_init( lock );
}

/* to perform a wait operation on the semaphore */
void pipe_sem_wait( pipe_sem_t *sem ) {
	sem->value--;
	if(sem->value <0) {
		lock_acquire( lock );
	}
}

/* to perform a signal operation on the semaphore */
void pipe_sem_signal( pipe_sem_t *sem ) {
	sem->value++;
	if( sem->value <=0 ) {
		lock_release( lock );
	}
}

/* called when releasing a lock */ 
void lock_release (int fd[2]) { 
	int pid; 
	pid = fork(); 
	if(pid<0) { 
		perror("fail to implement unlock\n"); 
		exit(1); 
	} 
	if(pid == 0) { 
		write(fd[1],"ok",10); 
		exit(0); 
	} 
} 

/* called when acquiring a lock */
void lock_acquire(int fd[2]) { 
	char buf[10]; 
	read(fd[0],buf,10); 
}

/* called when initializing a lock */
void lock_init(int fd[2]) { 
	if(pipe(fd) != 0) { 
		perror("fail to initialize pipe\n"); 
		exit(1); 
	} 
	lock_release(fd); 
}
