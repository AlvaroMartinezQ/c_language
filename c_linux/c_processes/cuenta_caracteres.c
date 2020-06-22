#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

//This program counts characters from a file using threads, processes and pipes

int p[2];

void funcion_creador(char* file_name);
void funcion_consumidor();

int main(int argc, char* argv[]){
    //printf("%s", argv[1]);
    pid_t pid_consumidor, pid_creador;
    pipe(p);
    pid_creador = fork();
    if(pid_creador < 0){
        printf("Fallo el fork.\n");
        exit(1);
    }else if(pid_creador == 0){
        close(p[0]);
        funcion_creador(argv[1]);
        close(p[1]);
        exit(0);
    }else{
        pid_consumidor = fork();
        if(pid_creador < 0){
            printf("Fallo el fork.\n");
            exit(1);
        }else if(pid_creador == 0){
            close(p[1]);
            funcion_consumidor();
            close(p[0]);
            exit(0);
        }else{
            wait(NULL);
        }
    }
    return 0;
}

void funcion_creador(char* file_name){
    FILE* file = fopen(file_name, "r");
    size_t n = 0;
    int c;
    if(file == NULL){
        printf("Fallo el fichero.\n");
        return;
    }
    while((c = fgetc(file)) != EOF){
        printf("%c ", c);
        write(p[1], &c, sizeof(c));
    }
    fclose(file);
    return;
}

void funcion_consumidor(){
    //printf("\n");
    char n;
    int contador = 0;
    while(n != '\0'){
        read(p[0], &n, sizeof(n));
        //printf("%c", n);
        contador++;
    }
    printf("longitud: %d\n", contador);
    return;
}