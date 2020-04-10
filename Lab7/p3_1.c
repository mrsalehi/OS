#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>

int global_param = 3;

void *kid(void *param){
	int local_param;

	printf("Thread %d, pid %d, addresses: &global: %X, &local:%X\n", pthread_self(), getpid(), &global_param, &local_param);
	global_param++;
	printf("In Thread %d, incremental global parameter=%d\n", pthread_self(), global_param);

	pthread_exit(0);
}

int main(){
	int local_param = 0;
	pthread_t threads[2];
	printf("In main thread, address: &global: %X\n", &global_param);
	int i, rc;
	if (fork() == 0){
		global_param = 200; local_param = 100;
	} else{
		for (i=0; i<2; i++)
			rc = pthread_create(&threads[i], NULL, kid, (void *)i);
		for (i=0; i<2; i++)
			pthread_join(threads[i], (void *)NULL);
		wait(NULL);
		printf("In main thread, global parameter: %d\n", global_param);
		printf("In main thread, local parameter: %d\n", local_param);
	}
	return 0;
}
