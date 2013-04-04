#include <stdio.h>
#include <stdlib.h>

#include "pipe_sem.h"

int SHARED_INTEGER = 0;
pipe_sem_t mutex;
pipe_sem_t wrt;
int readcount = 0;


#define WRITER_PRIORITY 0
#define READER_PRIORITY 1
#define NULL_NODE 0

struct node_t;
typedef struct node_t                                               
{                                                               
	  int priority;
	  pthread_t* tid;
      struct node_t *next;
	  struct node_t *prev;
} node_t; 

struct list_t;
typedef struct list_t
{
	int size;
	node_t *head;
} list_t;
void push(list_t* list, node_t* node);
int is_empty(list_t* list);
node_t* pop(list_t* list);
list_t* ready_queue;

void *reader_thread( void *x ) {
	printf("Reader thread <%d> starts\n", pthread_self());
	
	//lock
		pipe_sem_wait(&mutex);
		if(readcount == 0) pipe_sem_wait(&wrt);
		readcount++;
		pipe_sem_signal(&mutex);
	//lock
	
	//cs
		printf("Reader thread <%d> enters CS\n", pthread_self());
		//sleep(1);
		printf("Reader thread <%d> read the shared data as having value: %d\n", pthread_self(), SHARED_INTEGER);
		printf("Reader thread <%d> is exiting CS\n", pthread_self());
	//cs
	
	//unlock
		pipe_sem_wait(&mutex);
		readcount--;
		if(readcount == 0) signal(&wrt);
		pipe_sem_signal(&mutex);
	//unlock
	
	printf("Reader thread <%d> ends\n", pthread_self());
}

void *writer_thread( void *x ) {
	printf("Writer thread <%d> starts\n", pthread_self());

	//lock
		pipe_sem_wait(&wrt);
	//lock
	
	//cs
		printf("Writer thread <%d> enters CS\n", pthread_self());
		//sleep(1);
		++SHARED_INTEGER;
		printf("Writer thread <%d> incrementing shared data value by one to: %d\n", pthread_self(), SHARED_INTEGER);
		printf("Writer thread <%d> is exiting CS\n", pthread_self());
	//cs
	
	//unlock
		pipe_sem_signal(&wrt);
	//unlock

	printf("Writer thread <%d> ends\n", pthread_self());
}

int main(int argc, char** args) {
	
	// a variable for looping
	int i;

	// number of arriving threads
	int n_arriving_threads = 0;

	// a sequence of 0 and 1 separated by a blank space
	int* sequence = 0;

	// thread arrival interval
	int thread_arrival_interval = 0;
	
	pthread_t *th; 

	// extract number of arriving threads from program arguments
	n_arriving_threads = atoi( args[1] );
	
	// extract sequence from program arguments
	sequence = (int*) malloc(sizeof(int) * n_arriving_threads);
	if(!sequence) exit( -1 );	
	for(i = 0 ; i < n_arriving_threads ; i++) {
		sequence[i] = atoi (args[i + 2]);
	}
	
	th = (pthread_t*) malloc(sizeof(pthread_t) * n_arriving_threads);
	if(!sequence) exit( -1 );
	
	
	// extract thread arrival interval from program arguments
	thread_arrival_interval = atoi(args[n_arriving_threads + 2]);
	
	
	pipe_sem_init(&mutex,1);
	pipe_sem_init(&wrt,1);
	
	ready_queue = (list_t*)malloc(sizeof(list_t));
	if(!ready_queue) return;
	
	for(i = 0 ; i < n_arriving_threads ; i++) {
		if(sequence[i] == 1) {
			pthread_create(&th[i],NULL,writer_thread,NULL);
			
		}
		else {
			pthread_create(&th[i],NULL,reader_thread,NULL);
		}
		sleep(thread_arrival_interval);
	}

	printf("final value:  %d\n", SHARED_INTEGER);
	
	// return 0 to indicate successfull execution
	return 0;
}









/**
* Name:			push( list_t*, node_t*  )
* Description:  This function pushes a node on to the end of a linked list
* Params:		list_t* - a pointer to a head of a linked list
				node_t* - a pointer to a node to be pushed on to the end of the linked list
* Return:		void
*/
void push(list_t* list, node_t* node)
{
	if( list->size == 0 )
	{
		node->prev = NULL_NODE;
		node->next = NULL_NODE;
		list->head = node;
	}
	else
	{
		node_t* cur = list->head;
		while( cur->next != NULL_NODE )
		{
			cur = cur->next;
		}
		
		cur->next = node;
		node->prev = cur;
	}
	list->size++;
}

/**
* Name:			is_empty( list_t* )
* Description:  This function checked to see if a list_t is empty
* Params:		list_t*
* Return:		int - 0 if empty
*/
int is_empty(list_t* list)
{
	return list->size == 0;
}

/**
* Name:			pop( list_t* )
* Description:  This function poppes the node off the list with the lowest priority
* Params:		list_t* - The list
* Return:		node_t* - The node that has the lowest prioirty
*/
node_t* pop(list_t* list)
{
	int min;
	int min_index = 0;
	node_t* cur = list->head;
	int i = 0;

	if( list->size == 0 )
	{
		min = -1;
	}
	else if( list->size == 1 )
	{
		min = 0;
	}
	else
	{
		
		while(cur->next != NULL_NODE && i < list->size)
		{
			if(cur->next->priority < cur->priority)
			{
				min_index = i+1; 
			}
			i++;
			cur = cur->next;
			
		}
		min = min_index;
	}

	i = 0;
	cur = list->head;
	while(i < min)
	{
		cur = cur->next;
		i++;
	}
	if(cur == list->head)
	{
		list->head = cur->next;
	}
	else if(cur->next == NULL_NODE)
	{
		cur->prev->next = NULL_NODE;
	}
	else
	{
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;
	}
	list->size--;
	return cur;
}