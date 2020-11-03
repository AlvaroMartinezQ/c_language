#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

//'A' process writes on pipe, 'B' process reads from it 
//when both are finished 'C' runs and finishes too

int p[2];

void tareaA();
void tareaB();
void tareaC();

int main(){
    int pid;
    pipe(p);
    pid = fork();
    if(pid<0){
        printf("fallo el fork\n");
        exit(1);
    }else if(pid == 0){
        close(p[0]);
        tareaA();
        close(p[1]);
        exit(0);
    }else{
        close(p[1]);
        tareaB();
        close(p[0]);
        wait(NULL);
        tareaC();
    }
    return 0;
}

void tareaA(){
    int n = 10;
    int stop = -1;
    int i;
    for(i = 0; i < n; i++){
        write(p[1], &i, sizeof(i));
    }
    write(p[1], &stop, sizeof(stop));
    return;
}

void tareaB(){
    int n;
    while(n>=0){
        read(p[0], &n, sizeof(n));
        printf("%d\n", n);
    }
    return;
}

void tareaC(){
    printf("A y B terminaron, yo soy C. Tambien he terminado.\n");
    return;
}
