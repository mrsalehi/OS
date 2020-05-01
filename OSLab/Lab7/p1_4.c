#include <stdio.h>
#include <pthread.h>

int oslab = 0;

void *print_globalvar(void *threadid){
	int tid = (int) threadid;
	oslab = 5;
	printf("Inside thread %d. oslab is %d.\n", tid, oslab);
}

int main(){
	pthread_t thread;
	oslab = 4;
	int rc;
	rc = pthread_create(&thread, NULL, print_globalvar, (void *)0);
	printf("Inside main thread (before join). oslab is %d.\n", oslab);
	rc = pthread_join(thread, (void *)NULL);
	printf("Inside main thread (after join). oslab is %d.\n", oslab);
	return 0;
}
