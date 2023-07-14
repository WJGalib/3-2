#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main() {
    fork();
    fork();
    fork();
    printf("hello\n");
    printf("world\n");
    return 0;
}