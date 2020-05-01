#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int global_var = 0;

int main(){
  int local_var = 1;
  int *p = (int*) malloc(2);
  *p = 5;

  if (fork() > 0){
    local_var = 10; global_var = 20;
    printf("parent: local_var=%d, global_var=%d\n", local_var, global_var);
    *p = 50;
    printf("parent: p = %p and *p is %d\n", p, *p);
  } else {
    printf("child: local_var=%d, global_var=%d\n", local_var, global_var);
    printf("child: p = %p, *p = %d\n", p, *p);
  }

  return 0;
}
