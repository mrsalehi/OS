#include <stdio.h>
#include <pthread.h>


int sum = 0;
pthread_mutex_t mutexsum;

void *calcsum (void *number){
	int num = (int) number;
	pthread_mutex_lock(&mutexsum);
	sum += num;
	pthread_mutex_unlock(&mutexsum);
	pthread_exit(NULL);
}

int main(){
	int n;
	pthread_attr_t attr;
	pthread_mutex_init(&mutexsum, NULL);
	pthread_attr_init(&attr);
	printf("Enter n: ");
	scanf("%d", &n);
	pthread_t threads[n];
	int i, rc;
	for(i=0; i<n; i++)
		rc = pthread_create(&threads[i], &attr, calcsum, (void*)(i+1));

	pthread_attr_destroy(&attr);

	for (i=0; i<n; i++)
		pthread_join(threads[i], (void *)NULL);
	printf("sum of 1 to %d is %d\n", n, sum);

	pthread_mutex_destroy(&mutexsum);

	return 0;
}
