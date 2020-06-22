#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#define N 3
#define NCOCHES 10

int buffer[N];
pthread_mutex_t mutex;
pthread_cond_t lleno;
int numTotal = 0;

int asignarPlaza();
void imprimir();
void *parking(void *n);

int main(int argc, char* argv[]){
    pthread_t c[NCOCHES];
    int ids[NCOCHES];
    srand(time(NULL));
    int num = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&lleno, NULL);
    for(size_t i = 0 ; i < N; i++){
        buffer[i] = 0;
    }
    for(int i = 0; i < NCOCHES; i++){
        ids[i] = i;
        pthread_create(&c[i], NULL, (void *)parking, (void *)&ids[i]);
    }
    for(int i = 0; i < NCOCHES; i++){
        pthread_join(c[i], NULL);
    }
    return 0;
}

void *parking(void *n){
    int numcoche = *(int *) n;
    while(1){
        printf("Soy el coche %d, esperando a entrar.\n", numcoche);
        pthread_mutex_lock(&mutex);
        while(numTotal == N){
            printf("Coche %d, el parking esta lleno espere.\n", numcoche);
            pthread_cond_wait(&lleno, &mutex);
        }
        int plaza = asignarPlaza();
        buffer[plaza] = 1;
        numTotal++;
        printf("Coche %d, tienes asignada la plaza %d.\n", numcoche, plaza);
        pthread_mutex_unlock(&mutex);
        sleep(rand() % 10);
        printf("Coche %d, abandonando el parking. PLaza %d libre.\n", numcoche, plaza);
        pthread_mutex_lock(&mutex);
        buffer[plaza] = 0;
        numTotal--;
        pthread_cond_broadcast(&lleno);
        pthread_mutex_unlock(&mutex);
        sleep(rand() % 20);
    }
}

int asignarPlaza(){
    for(size_t i = 0; i < N; i++){
        if(buffer[i] == 0){
            return i;
        }
    }
    return -1;
}

void imprimir(){
    /*
    printf("PLAZAS");
    for(size_t i = 0; i < N; i++){
        printf("%d", buffer[i]);
    }
    printf("\n");
    */
}