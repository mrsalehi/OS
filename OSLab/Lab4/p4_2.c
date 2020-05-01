#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	char *args[4];
	args[0] = "ls";
	args[1] = "-g";
	args[2] = "-h";
	args[3] = NULL; 	
	
	if (fork() == 0){
		execvp(args[0], args);
	} else {
		wait(NULL);
	}


	return 0;
}
