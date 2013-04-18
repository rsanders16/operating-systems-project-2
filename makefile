all:
	gcc -c pipe_sem.c -o pipe_sem.o
	gcc -c rw_test.c -o rw_test.o
	gcc -lpthread rw_test.o pipe_sem.o -o rw_test
	clear
	./rw_test 5 0 0 1 1 0 1