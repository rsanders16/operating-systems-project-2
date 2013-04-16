#include <stdio.h>
#include <stdlib.h>

#include "pipe_sem.h"

int SHARED_INTEGER = 0;
pipe_sem_t mutex;

#define WRITER_PRIORITY 0
#define READER_PRIORITY 1
#define NULL_NODE 0

struct node_t;
typedef struct node_t                                               
{                                                               
	  long tid;
      struct node_t *next;
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


list_t* readerQ;
list_t* writerQ;

void *reader_thread( void *x ) {

	//printf("Reader thread <%ld> starts\n", pthread_self());
	
	//lock
		pipe_sem_wait(&mutex);	
		
		node_t* new_node;
		new_node = (node_t*) malloc(sizeof(node_t));
		if(!new_node) exit( -1 );
		new_node->tid = pthread_self();
		push(readerQ, new_node);
		if(is_empty(writerQ) && is_empty(readerQ))
		{
			// do nothing
		}
		else if(is_empty(writerQ) && !is_empty(readerQ)) 
		{
			pthread_yield(pop(readerQ)); 
		}
		else if(!is_empty(writerQ) && is_empty(readerQ)) 
		{
			pthread_yield(pop(writerQ)); 
		}
		else if(!is_empty(writerQ) && !is_empty(readerQ)) 
		{
			pthread_yield(pop(writerQ)); 
		}
		
	//lock

	
	//cs
		printf("Reader thread <%ld> enters CS\n", pthread_self());
		//sleep(1);
		//printf("Reader thread <%ld> read the shared data as having value: %d\n", pthread_self(), SHARED_INTEGER);
		printf("Reader thread <%ld> is exiting CS\n", pthread_self());
	//cs
	
	//unlock
		pipe_sem_signal(&mutex);
	//unlock
	
	//printf("Reader thread <%ld> ends\n", pthread_self());
}

void *writer_thread( void *x ) {
	//printf("Writer thread <%ld> starts\n", pthread_self());
	//lock
		pipe_sem_wait(&mutex);
		
		node_t* new_node;
		new_node = (node_t*) malloc(sizeof(node_t));
		if(!new_node) exit( -1 );
		new_node->tid = pthread_self();
		push(writerQ, new_node);
		if(is_empty(writerQ) && is_empty(readerQ))
		{
			// do nothing
			
		}
		else if(is_empty(writerQ) && !is_empty(readerQ)) 
		{
			
			pthread_yield(pop(writerQ)); 
		}
		else if(!is_empty(writerQ) && is_empty(readerQ)) 
		{
			pthread_yield(pop(writerQ)); 
		}
		else if(!is_empty(writerQ) && !is_empty(readerQ)) 
		{
			pthread_yield(pop(writerQ)); 
		}
	//lock
	
	//cs
		printf("Writer thread <%ld> enters CS\n", pthread_self());
		//sleep(1);
		SHARED_INTEGER++;
		//printf("Writer thread <%ld> incrementing shared data value by one to: %d\n", pthread_self(), SHARED_INTEGER);
		printf("Writer thread <%ld> is exiting CS\n", pthread_self());
	//cs
	
	//unlock
		pipe_sem_signal(&mutex);
	//unlock

	//printf("Writer thread <%ld> ends\n", pthread_self());
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
	
	
	readerQ = (list_t*)malloc(sizeof(list_t));
	if(!readerQ) return;
	
	writerQ = (list_t*)malloc(sizeof(list_t));
	if(!writerQ) return;
	
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
	node_t* to_return;
	if( list->size == 0 )
	{
		return NULL_NODE;
	}
	else
	{
		to_return = list->head;
		list->head = to_return->next;
	}
	list->size--;
	return to_return;
}