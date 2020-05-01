#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	if (fork() == 0){
		sleep(2);
		int parent_id = getppid();
		printf("PID of the child's parent is %d", parent_id);
	} else {

	}

	return 0;
}
