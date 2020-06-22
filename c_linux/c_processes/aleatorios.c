#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

//This program produces random numbers
//using pipes and creation of new processes

int p[2];

void manejadorHijo(int sig){
    int num;
    if(sig == SIGUSR2){
        exit(0);
    }else{
        num = 1 + (int) (10.0 * rand()/(RAND_MAX+1.0));
        write(p[1], &num, sizeof(int));
    }
}

int main(int argc, char **argv){
    pid_t pid;
    char buf[1024];
    int num;
    signal(SIGUSR1, manejadorHijo);
    signal(SIGUSR2, manejadorHijo);
    pipe(p);
    srand(time(NULL));
    pid = fork();
    if(pid == 0){
        close(p[0]);
        while(1);
    }else if(pid > 0){
        close(p[1]);
        while(fgets(buf, 1024, stdin) != NULL){
            kill(pid, SIGUSR1);
            read(p[0], &num, sizeof(int));
            printf("%d", num);
        }

        kill(pid, SIGUSR2);
        wait(NULL);
    }
    return 0;
}