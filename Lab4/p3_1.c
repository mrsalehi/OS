#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	if (fork() == 0){
		int i;
		for (i = 1; i <= 100; i++){
			printf("%d\n", i);
		}
	} else {
		wait(NULL);
		printf("child's counting is finished!\n");
	}

	return 0;
}
