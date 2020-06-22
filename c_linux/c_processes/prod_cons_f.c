#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

//Program to read from one file using pipes and signals

void producer(FILE * f, int *pipedesc);
void consumer(FILE * f, int *pipedesc);
void killer(pid_t pid_producer, pid_t pid_consumer, FILE * f_in, FILE * f_out);


int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Task failed: %s, use: %s FILE_1 FILE_2\n", argv[0], argv[0]);
        return -2;
    }
    pid_t pid_producer;
    pid_t pid_consumer;
    pid_t pid_killer;
    int pipedes[2];
    FILE * f_in;
    FILE *f_out;
    if((f_in = fopen(argv[1], "r")) == NULL){
        printf("Failed opening FILE: %s\n", argv[1]);
        return -3;
    }
    if((f_out = fopen(argv[2], "w")) == NULL){
        printf("Failed opening FILE: %s\n", argv[2]);
        return -3;
    }
    pipe(pipedes);
    pid_producer = fork();
    if(pid_producer < 0){
        printf("Fork failed.\n");
        return -1;;
    }else if(pid_producer == 0){
        producer(f_in, pipedes);
        return 0;
    }
    pid_consumer = fork();
    if(pid_consumer < 0){
        printf("Fork failed.\n");
        return -1;
    }else if(pid_consumer == 0){
        consumer(f_out, pipedes);
        return 0;
    }
    close(pipedes[0]);
    close(pipedes[1]);
    pid_killer = fork();
    if(pid_killer < 0){
        printf("Fork failed.\n");
        return -1;
    }else if(pid_killer == 0){
        killer(pid_producer, pid_producer, f_in, f_out);
        return 0;
    }
    waitpid(pid_producer, NULL, 0);
    waitpid(pid_consumer, NULL ,0);
    kill(pid_killer, SIGKILL);
    return 0;
}

void producer(FILE * f, int *pipedesc){
    char line[512];
    FILE * f_pipe;
    close(pipedesc[0]);
    f_pipe = fdopen(pipedesc[1], "w");
    while(fgets(line, 512, f) != NULL){
        fprintf(f_pipe, "%d\n", atoi(line));
    }
    fclose(f);
    fclose(f_pipe);
    return;
}

void consumer(FILE * f, int *pipedesc){
    char line[512];
    FILE *f_pipe;
    int number = 0, n;
    close(pipedesc[1]);
    f_pipe = fdopen(pipedesc[0], "r");
    while(fgets(line, 512, f_pipe) != NULL){
        number++;
        if((n = atoi(line)) % 2 == 0){
            fprintf(f, "Line: %d, number: %d\n", number, n);
        }
    }
    fclose(f_pipe);
    fclose(f);
    return;
}

void killer(pid_t pid_producer, pid_t pid_consumer, FILE * f_in, FILE * f_out){
    sleep(100);
    printf("Time expired. Killing processes.\n");
    kill(pid_producer, SIGKILL);
    kill(pid_consumer, SIGKILL);
    kill(getpid(), SIGKILL);
    exit(1);
}