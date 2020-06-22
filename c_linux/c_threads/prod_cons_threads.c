#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

//Program to read from one file using threads, pipes and signals

void *producer(void *param);
void *consumer(void *param);
void killer_funct(void);

int pipedes[2];
FILE *f_in, *f_out;

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Task failed: %s, use: %s FILE_1 FILE_2\n", argv[0], argv[0]);
        return -2;
    }
    if((f_in = fopen(argv[1], "r")) == NULL){
        printf("Failed opening FILE: %s\n", argv[1]);
        return -3;
    }
    if((f_out = fopen(argv[2], "w")) == NULL){
        printf("Failed opening FILE: %s\n", argv[2]);
        return -3;
    }
    pthread_t tid_producer, tid_consumer;
    pid_t killer;
    pipe(pipedes);
    if(pthread_create(&tid_producer, NULL, producer, NULL) != 0){
        printf("Creation of producer thread failed\n");
        return -1;
    }
    if(pthread_create(&tid_consumer, NULL, consumer, NULL) != 0){
        printf("Creation of consumer thread failed\n");
        return -1;
    }
    killer = fork();
    if(killer < 0){
        printf("Killer process fork failed\n");
        return -4;
    }else if(killer == 0){
        killer_funct();
        return 0;
    }
    pthread_join(tid_producer, NULL);
    pthread_join(tid_consumer, NULL);
    kill(killer, SIGKILL);
    return 0;
}

void *producer(void *param){
    char line[512];
    FILE * f_pipe;
    f_pipe = fdopen(pipedes[1], "w");
    while(fgets(line, 512, f_in) != NULL){
        fprintf(f_pipe, "%d\n", atoi(line));
    }
    fclose(f_in);
    fclose(f_pipe);
    pthread_exit(NULL);
}

void *consumer(void *param){
    char line[512];
    FILE * f_pipe;
    int number = 0, n;
    f_pipe = fdopen(pipedes[0], "r");
    while(fgets(line, 512, f_pipe) != NULL){
        number++;
        if((n = atoi(line)) % 2 == 0){
            fprintf(f_out, "Line %d, number %d.\n", number, n);
        }
    }
    fclose(f_out);
    fclose(f_pipe);
    pthread_exit(NULL);
}

void killer_funct(void){
    close(pipedes[0]);
    close(pipedes[1]);
    sleep(100);
    printf("Time has expired, stopping program...\n");
    kill(getpid(), SIGKILL);
    exit(1);
}