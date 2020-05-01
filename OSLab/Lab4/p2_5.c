#include <stdio.h>
#include <unistd.h>

int main(){
	printf("Hello World!\n");
	fork();
	printf("I am after first fork\n");
	fork();
	printf("I am after second fork\n");
	fork();
	printf("I am after third fork\n");
	return 0;
}
