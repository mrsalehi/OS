#include <unistd.h>
#include <stdio.h>

int main(){
	int id = getppid();
	printf("%d\n", id);

return 0;
}
