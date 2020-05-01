#include <stdio.h>
#include <pthread.h>

void *printHello(void *threadid){
	long tid;
	tid = (long)threadid;
	printf("Hello World! It is thread #%ld\n", tid);
	pthread_exit(NULL);
}

int main(){
	int n = 10;
	pthread_t threads[n];
	long t;
	int rc;
	for (t=0; t<n; t++){
		rc = pthread_create(&threads[t], NULL, printHello, (void*) t);
		if (rc){
			printf("Error!\n");
		}
	}
	pthread_exit(NULL);
}
