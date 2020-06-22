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

/***************************************
6 coballas
1 plato donde pueden comer 3
1 rueda donde puede correr 1
primero comen, luego utilizan la rueda y
vuelta a empezar.
***************************************/

#define NCOBALLAS 6
#define NCOMER 3
#define NRUEDA 1

int buffer_plato[NCOMER];
int buffer_rueda[NRUEDA];

pthread_mutex_t mutex_comer;
pthread_cond_t cond_comer;

pthread_mutex_t mutex_rueda;
pthread_cond_t cond_rueda;

int numero_comiendo;
int numero_rueda;

int asignar_espacio_plato();
void *acciones(void* n);

int main(int argc, char* argv[]){
    //Creamos los threads
    pthread_t coballas[NCOBALLAS];
    int ids[NCOBALLAS];
    srand(time(NULL));
    
    pthread_mutex_init(&mutex_comer, NULL);
    pthread_cond_init(&cond_comer, NULL);

    pthread_mutex_init(&mutex_rueda, NULL);
    pthread_cond_init(&cond_rueda, NULL);

    //Inicializamos los buffers
    for(size_t i = 0 ; i < NCOMER; i++){
        buffer_plato[i] = 0;
    }
    for(size_t i = 0 ; i < NRUEDA; i++){
        buffer_rueda[i] = 0;
    }

    //Inicializamos los threads o coballas
    for(int i = 0; i < NCOBALLAS; i++){
        ids[i] = i;
        //El ultimo argumento es para saber que coballa le estamos pasando
        pthread_create(&coballas[i], NULL, (void *)acciones, (void *)&ids[i]);
    }
    //Unimos threads
    for(int i = 0; i < NCOBALLAS; i++){
        pthread_join(coballas[i], NULL);
    }
    return 0;
}

void *acciones(void* n){
    int num_coballa = *(int *) n;
    while(1){

        //VAMOS A INTENTAR COMER

        printf("Soy la coballa %d, quiero comer.\n", num_coballa);
        pthread_mutex_lock(&mutex_comer);
        while(numero_comiendo == NCOMER){
            //Si esta lleno el plato esperamos
            printf("Coballa %d esperando a comer.\n", num_coballa);
            pthread_cond_wait(&cond_comer, &mutex_comer);
        }
        int posicion = asignar_espacio_plato();
        buffer_plato[posicion] = 1;
        numero_comiendo++;
        printf("Coballa %d comiendo en la posicion del plato %d.\n", num_coballa, posicion);
        pthread_mutex_unlock(&mutex_comer);
        sleep(rand() % 10);
        printf("Coballa %d esta llena.\n", num_coballa);
        pthread_mutex_lock(&mutex_comer);
        buffer_plato[posicion] = 0;
        numero_comiendo--;
        pthread_cond_broadcast(&cond_comer);
        pthread_mutex_unlock(&mutex_comer);
        sleep(rand() % 20);

        // AHORA VAMOS A INTENTAR CORRER

        printf("Soy la coballa %d, quiero correr.\n", num_coballa);
        pthread_mutex_lock(&mutex_rueda);
        while(numero_comiendo == NRUEDA){
            //Si esta llena la rueda esperamos
            printf("Coballa %d esperando a correr.\n", num_coballa);
            pthread_cond_wait(&cond_rueda, &mutex_rueda);
        }
        int posicionRueda = 1;
        buffer_rueda[posicionRueda] = 1;
        numero_rueda++;
        printf("Coballa %d corriendo.\n", num_coballa);
        pthread_mutex_unlock(&mutex_rueda);
        sleep(rand() % 10);
        printf("Coballa %d esta cansada, se va a dormir.\n", num_coballa);
        pthread_mutex_lock(&mutex_rueda);
        buffer_rueda[posicionRueda] = 0;
        numero_rueda--;
        pthread_cond_broadcast(&cond_rueda);
        pthread_mutex_unlock(&mutex_rueda);
        //Para no pedir comer inmediatamente despues 
        //de terminar de correr le mandamos a dormir
        sleep(rand() % 20);
    }
}

int asignar_espacio_plato(){
    for(size_t i = 0; i < NCOMER; i++){
        if(buffer_plato[i] == 0){
            return i;
        }
    }
    return -1;
}