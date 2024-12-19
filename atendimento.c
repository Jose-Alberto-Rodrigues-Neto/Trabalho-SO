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

void satisfacao_cliente(long tempo_de_espera, long paciencia){
    if(tempo_de_espera <= paciencia){
        printf("Cliente satisfeito");
    }else{
        printf("Cliente insatisfeito");
    }
}

void *atendente(void* arg) {
        //forma de pegar os argumentos do struct
        AtendenteArgs* args = (AtendenteArgs*)arg;

        int num_clientes = args->num_clientes;
        int fila_aux = 1
        if(fila_aux == 1) {
            Fila* clientes = args->alta_prioridade;
        } else {
            Fila* clientes = args->baixa_prioridade;
        }

        struct timeval tv;
        long int wait_time;
        Cliente* cliente = clientes->front->cliente;
        pid_t analista;
        pid_t cpid = cliente->pid;
        kill(cpid, SIGCONT);

        sem_wait(sem_atend);
        sem_wait(sem_block);

        FILE *lng = fopen("LNG.txt", "a");
        fprintf(lng, "%d\n", cpid);
        fclose(lng);

        if(gettimeofday(&tv, NULL) != 0) {
                perror("erro no gettimeofday\n");
        }

        wait_time = tv.tv_usec - cliente->chegada;

        satisfacao_cliente(wait_time, cliente->prioridade);
        desenfileirar(clientes);
        analista = fork();
        if(analista < 0){
            printf("Erro em criar processo analista!\n");
        }else if(analista == 0){
            execlp("./analista", "analista", NULL);
            //desbloquei processo analista
            sem_open("/sem_block", O_CREAT | O_EXCL, 0644, 1);
        }
        FILE *analista = fopen("analista.txt", "r");
        pid_t apid;
        fscanf(analista, "%d\n", &apid);
        fclose(analista);

        kill(apid, SIGCONT);

        if(fila_aux == 1) {
            fila_aux == 0;
        } else {
            fila_aux == 1;
        }
}

//estrutura dos argumentos passados para a thread recepcao

void* recepcao( void* arg) {
    //recebe os argumentos da função
    RecepcaoArgs* args = (RecepcaoArgs*)arg;

    //cria semáforo
    sem_open("/sem_atend", O_CREAT | O_EXCL, 0644, 1);
    sem_open("/sem_block", O_CREAT | O_EXCL, 0644, 1);
    
    pid_t criarCliente;

    int clientes = args->num_clientes;

    Fila* fila_baixa_prioridade = args->baixa_prioridade;

    Fila* fila_alta_prioridade = args->alta_prioridade;

    srand(time(NULL));
    
    if(clientes == 0){
        
        char input;

        while(1) {
            
            criarCliente = fork();

            if(criarCliente < 0){
                printf("Erro ao criar o processo filho!\n");
            }else if(criarCliente == 0){

                //cria novo cliente
                Cliente* novoCliente = (Cliente*)malloc(sizeof(Cliente));
                if (novoCliente == NULL) {
                    perror("Erro ao alocar memória para novo cliente");
                    exit(EXIT_FAILURE);
                }

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
                Cliente* novoCliente = (Cliente*)malloc(sizeof(Cliente));
                if (novoCliente == NULL) {
                    perror("Erro ao alocar memória para novo cliente");
                    exit(EXIT_FAILURE);
                }

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