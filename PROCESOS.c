#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_NUM 10000

int main() {
    int pipe_padre_hijo[2]; 
    int pipe_hijo_padre[2]; 
    char testigo = 'T'; 

    if (pipe(pipe_padre_hijo) == -1 || pipe(pipe_hijo_padre) == -1) {
        perror("Error al crear las tuberías");
        exit(1);
    }

    pid_t pid = fork(); 

    if (pid < 0) {
        perror("Error en fork()");
        exit(1);
    } 
    else if (pid == 0) {
        // --- PROCESO HIJO ---
        close(pipe_padre_hijo[1]); 
        close(pipe_hijo_padre[0]); 

        for (int i = 1; i <= MAX_NUM; i++) {
            // 1. Esperar turno del padre
            read(pipe_padre_hijo[0], &testigo, 1);
            
            // 2. Imprimir y forzar salida a pantalla
            printf("El hijo dice... %d\n", i);
            fflush(stdout); // <--- ESTO FUERZA EL INTERCALADO VISUAL
            
            // 3. Devolver turno al padre
            write(pipe_hijo_padre[1], &testigo, 1);
        }

        close(pipe_padre_hijo[0]);
        close(pipe_hijo_padre[1]);
        exit(0);
    } 
    else {
        // --- PROCESO PADRE ---
        close(pipe_padre_hijo[0]); 
        close(pipe_hijo_padre[1]); 

        for (int i = 1; i <= MAX_NUM; i++) {
            // 1. Imprimir y forzar salida a pantalla
            printf("El padre dice... %d\n", i);
            fflush(stdout); // <--- ESTO FUERZA EL INTERCALADO VISUAL
            
            // 2. Dar turno al hijo
            write(pipe_padre_hijo[1], &testigo, 1);
            
            // 3. Esperar a que el hijo termine su turno
            read(pipe_hijo_padre[0], &testigo, 1);
        }

        wait(NULL);
        close(pipe_padre_hijo[1]);
        close(pipe_hijo_padre[0]);
    }

    return 0;
}
