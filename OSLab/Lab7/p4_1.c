#include <stdio.h>
#include <pthread.h>
#include <string.h>

typedef struct thdata{
	int thread_no;
	char message[100];
} stdata;


void* print(void* threadarg){
	stdata *data;
	data = (stdata *) threadarg;
	printf("thread_no: %d, Message: %s\n", data->thread_no, data->message);

	pthread_exit(NULL);
}


int main(){
	stdata data[2];
	pthread_t threads[2];

	data[0].thread_no = 0; strcpy(data[0].message, "Heyyyy!");
	data[1].thread_no = 1; strcpy(data[1].message, "What's up?");

	int i, rc;
	for (int i=0; i<2; i++)
		rc = pthread_create(&threads[i], NULL, print, (void*) &data[i]);

	pthread_exit(NULL);

	return 0;
}
