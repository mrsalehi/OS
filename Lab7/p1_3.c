#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *printpid(void *threadid){
	int tid = (int) threadid;
	printf("Inside thread %d. PID is %d.\n", tid, getpid());
}

int main(){
	pthread_t thread;
	int rc;
	rc = pthread_create(&thread, NULL, printpid, (void *)0);
	printf("Inside main thread. PID is %d.\n", getpid());
	rc = pthread_join(thread, (void *)NULL);
	return 0;
}
