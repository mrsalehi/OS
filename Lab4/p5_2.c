#include <stdio.h>
#include <unistd.h>

int main(){
	fork();
	fork();
	printf("Parent Process ID is %d\n", getppid());
	return 0;
}
