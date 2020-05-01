#include <stdio.h>
#include <unistd.h>

int main(){
	int i, ret;
	for(i=0; i<4; i++){
		ret = fork();
		if (ret < 0){
			printf("Error!\n");
		}
		printf("%d,%p,%d,%d\n", i, &i, getpid(), getppid());
	}

	return 0;
}
