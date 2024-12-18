/* TODO: 
    Recepção deve:
    - criar  semáforos "\sem_atend" e "\sem_block"
    - Cria N ou infinitos processos clientes
    - Cada processo é criado com uma prioridade, entre duas possíveis -> Prioridade aleatória: 50% para cada, a prioridade alta é X/2 e a baixa é X, com input de entrada
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "recepcao.h"
#include <semaphore.h>
#include <fcntl.h>

// Prioridade é o tempo máximo em 'ms' que os clientes, em geral, podem esperar.
void startRecepcao(int clientes, int *prioridade, int *fila) {
    
    pid_t novoCliente;

    if(clientes == 0){
        
        int t = 1;
        
        while(t) {
            novoCliente = fork();

            if(novoCliente < 0){
                printf("Erro ao criar o processo filho!\n");
            }else if(novoCliente == 0){
                int processPID = getpid();
                prioridade
                printf("Eu sou o filho, processo: %d\n", processPID);
                execlp("./cliente", "cliente", NULL); //cria processo cliente
                sem_create("/sem_atend", O_CREAT | O_EXCL, 0644, 1);
                //criar forma de adicionar pid do cliente a lista
            }
        }

    } else{

        for(int i = 0; i < clientes; i++){
            novoCliente = fork();

            if(novoCliente < 0){
                printf("Erro ao criar o processo filho!\n");
            }else if(novoCliente == 0){
                int processPID = getpid();
                prioridade
                printf("Eu sou o filho, processo: %d\n", processPID);
                execlp("./cliente", "cliente", NULL); //cria processo cliente
                sem_create("/sem_atend", O_CREAT | O_EXECL, 0644, 1);
                //criar forma de adicionar pid do cliente a lista
            }
        }

    }
}