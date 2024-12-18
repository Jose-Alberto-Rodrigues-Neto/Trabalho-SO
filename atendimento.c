#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include "fila.h"
#include "atendimento.h"


void *atendente(void *args) {
        //forma de pegar os argumentos do struct
        AtendenteArgs* args = (AtendenteArgs*)arg;

        int num_clientes = args->num_clientes;
        
        Fila* clientes = args->fila;

        //semáforos
        sem_t *sem_atend, *sem_block;

        struct timeval tv;
        long int wait_time;
        Cliente* cliente = clientes->front->cliente;

        pid_t cpid = cliente->pid;
        kill(cpid, SIGCONT);

        sem_wait(sem_atend);
        sem_wait(sem_block);
        sem_close(sem_block);

        FILE *lng = fopen("lng.txt", "a");
        fprintf(lng, "%d\n", cpid);
        fclose(lng);

        sem_open(sem_block);
        if(gettimeofday(&tv, NULL) != 0) {
                perror("erro no gettimeofday\n");
        }

        wait_time = tv.tv_usec - cliente->chegada;

        satisfacao_cliente(wait_time, cliente->prioridade);
        desenfileirar(clientes);

        FILE *analista = fopen("analista.txt", "r");
        pid_t apid;
        fscanf(analista, "%d\n", &apid);
        fclose(analista);

        kill(apid, SIGCONT);
}

//estrutura dos argumentos passados para a thread recepcao

void* recepcao( void* arg) {
    //recebe os argumentos da função
    RecepcaoArgs* args = (RecepcaoArgs*)arg;
    
    pid_t criarCliente;

    int clientes = args->num_clientes;

    Fila* fila_baixa_prioridade = args->baixa_prioridade;

    Fila* fila_alta_prioridade = args->alta_prioridade;

    srand(time(NULL));
    
    if(clientes == 0){
        
        char input;

        while(1) {
            input =
            criarCliente = fork();

            if(criarCliente < 0){
                printf("Erro ao criar o processo filho!\n");
            }else if(criarCliente == 0){

                //cria novo cliente
                Cliente* novoCliente;

                //atribui pid do cliente
                novoCliente->pid = getpid();

                //define prioridade randomicamente
                int prioridade = rand();
                if (prioridade % 2 == 0){
                    //se par, adiciona prioridade baixa
                    novoCliente->prioridade = args->paciencia;
                }else{
                    //se impar, adiciona prioridade alta
                    novoCliente->prioridade = (args->paciencia)/2;
                }
                
                long tempo_chegada = args->tempo_chegada;
                tempo_chegada = clock();

                novoCliente->chegada = tempo_chegada;

                //cria processo cliente
                execlp("./cliente", "cliente", NULL); 

                //cria semáforo
                sem_open("/sem_atend", O_CREAT | O_EXCL, 0644, 1);

                //adiciona cliente na lista
                if(novoCliente->prioridade < args->paciencia){
                    enfileirar(fila_alta_prioridade, novoCliente);
                }else{
                    enfileirar(fila_baixa_prioridade, novoCliente);
                }
                
                printf("Um novo cliente com o número de processo: %d\nFoi criado com sucesso.", novoCliente->pid);
            }
        }

    } else{

        for(int i = 0; i < clientes; i++){
            criarCliente = fork();

            if(criarCliente < 0){
                printf("Erro ao criar o processo filho!\n");
            }else if(criarCliente == 0){
                //cria novo cliente
                Cliente* novoCliente;

                //atribui pid do cliente
                novoCliente->pid = getpid();

                //define prioridade randomicamente
                int prioridade = rand();
                if (prioridade % 2 == 0){
                    //se par, adiciona prioridade baixa
                    novoCliente->prioridade = args->paciencia;
                }else{
                    //se impar, adiciona prioridade alta
                    novoCliente->prioridade = (args->paciencia)/2;
                }
                
                long tempo_chegada = args->tempo_chegada;
                tempo_chegada = clock();

                novoCliente->chegada = tempo_chegada;

                //cria processo cliente
                execlp("./cliente", "cliente", NULL); 

                //cria semáforo
                sem_open("/sem_atend", O_CREAT | O_EXCL, 0644, 1);

                //adiciona cliente na lista
                if(novoCliente->prioridade < args->paciencia){
                    enfileirar(fila_alta_prioridade, novoCliente);
                }else{
                    enfileirar(fila_baixa_prioridade, novoCliente);
                }
                
                printf("Um novo cliente com o número de processo: %d\nFoi criado com sucesso.", novoCliente->pid);
            }
        }

    }
}