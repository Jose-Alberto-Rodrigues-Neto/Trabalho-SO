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

void satisfacao_cliente(long tempo_de_espera, long paciencia, pid_t pid, FILE* file) { 
    if (((double)(tempo_de_espera) / CLOCKS_PER_SEC * 1000) <= paciencia) {
        fprintf(file, "Cliente PID=%d satisfeito\n", pid);
    } else {
        fprintf(file, "Cliente PID=%d insatisfeito\n", pid);
    }
}

void *atendente(void* arg) {
    printf("ATENDENTE INICIADO\n");
    AtendenteArgs* args = (AtendenteArgs*)arg;

    FILE* atendidos = fopen("atendidos.txt", "w"); // Modo append
    if (!atendidos) {
        perror("Erro ao abrir o arquivo atendidos.txt");
        return NULL;
    }

    while (1) {
        Fila* clientes = NULL;

        // Prioriza a fila alta
        sem_wait(args->sem_atend); // Protege a leitura das filas
        if (!fila_vazia(args->fila_alta)) {
            clientes = args->fila_alta;
        } else if (!fila_vazia(args->fila_baixa)) {
            clientes = args->fila_baixa;
        } else {
            sem_post(args->sem_atend);
            break; // Ambas as filas estão vazias
        }
        sem_post(args->sem_atend);

        // Desenfileira o cliente selecionado
        sem_wait(args->sem_atend);
        Cliente* cliente = desenfileirar(clientes);
        sem_post(args->sem_atend);

        if (!cliente) continue; // Ignora caso desenfileirar falhe

        printf("Atendendo cliente PID=%d\n", cliente->pid);

        // Calcula o tempo de espera e registra satisfação
        args->tempo_ini = clock();
        long wait_time = args->tempo_ini - cliente->chegada;

        satisfacao_cliente(wait_time, cliente->prioridade, cliente->pid, atendidos);

        free(cliente);
    }

    fclose(atendidos);
    printf("ATENDENTE FINALIZADO\n");
    return NULL;
}




//estrutura dos argumentos passados para a thread recepcao

void* recepcao( void* arg) {
    //recebe os argumentos da função
    RecepcaoArgs* args = (RecepcaoArgs*)arg;

    if (args->alta_prioridade == NULL || args->baixa_prioridade == NULL) {
        fprintf(stderr, "Filas não inicializadas corretamente na recepção.\n");
        pthread_exit(NULL);
    }

    //cria semáforo
    args->sem_atend = sem_open("/sem_atend", O_CREAT, 0644, 1);
    if (args->sem_atend == SEM_FAILED) {
        perror("Erro ao criar semáforo sem_atend");
        exit(EXIT_FAILURE);
    }

    args->sem_block = sem_open("/sem_block", O_CREAT, 0644, 1);
    if (args->sem_block == SEM_FAILED) {
        perror("Erro ao criar semáforo sem_block");
        exit(EXIT_FAILURE);
    }
    
    pid_t criarCliente;

    int clientes = args->num_clientes;

    Fila* fila_baixa_prioridade = args->baixa_prioridade;

    Fila* fila_alta_prioridade = args->alta_prioridade;

    srand(time(NULL));
    
    if(clientes == 0){
        
        char input;

        while(1) {
            
            criarCliente = fork();

            if (criarCliente < 0) {
                perror("Erro ao criar processo filho\n");
                exit(EXIT_FAILURE);
            } else if (criarCliente == 0) {
                // Processo filho
                printf("Um novo cliente com o número de processo: %d foi criado com sucesso.\n", getpid());
                execlp("./cliente", "cliente", NULL);
                exit(EXIT_SUCCESS); // Garanta que o processo filho não continue no pai
            } else {
                // Processo pai
                Cliente* novoCliente = (Cliente*)malloc(sizeof(Cliente));
                if (novoCliente == NULL) {
                    perror("Erro ao alocar memória para novo cliente");
                    exit(EXIT_FAILURE);
                }

                // Atribua o PID do processo filho
                novoCliente->pid = criarCliente;
                novoCliente->prioridade = (rand() % 2 == 0) ? args->paciencia : (args->paciencia) / 2;
                novoCliente->chegada = clock();
                printf("Novo cliente criado: PID=%d, Prioridade=%d, Chegada=%ld\n", novoCliente->pid, novoCliente->prioridade, novoCliente->chegada);

                // Adiciona cliente na lista correta
                if (novoCliente->prioridade < args->paciencia) {
                    enfileirar(args->alta_prioridade, novoCliente);
                    printf("Cliente adicionado à fila de alta prioridade.\n");
                } else {
                    enfileirar(args->baixa_prioridade, novoCliente);
                    printf("Cliente adicionado à fila de baixa prioridade.\n");
                }
            }

        }

    } else{

        for(int i = 0; i < clientes; i++){
            
            criarCliente = fork();

            if (criarCliente < 0) {
                perror("Erro ao criar processo filho\n");
                exit(EXIT_FAILURE);
            } else if (criarCliente == 0) {
                // Processo filho
                printf("Um novo cliente com o número de processo: %d foi criado com sucesso.\n", getpid());
                execlp("./cliente", "cliente", NULL);
                exit(EXIT_SUCCESS); // Garanta que o processo filho não continue no pai
            } else {
                // Processo pai
                Cliente* novoCliente = (Cliente*)malloc(sizeof(Cliente));
                if (novoCliente == NULL) {
                    perror("Erro ao alocar memória para novo cliente");
                    exit(EXIT_FAILURE);
                }

                // Atribua o PID do processo filho
                novoCliente->pid = criarCliente;
                novoCliente->prioridade = (rand() % 2 == 0) ? args->paciencia : (args->paciencia) / 2;
                novoCliente->chegada = clock();
                printf("Novo cliente criado: PID=%d, Prioridade=%d, Chegada=%ld\n", novoCliente->pid, novoCliente->prioridade, novoCliente->chegada);

                // Adiciona cliente na lista correta
                if (novoCliente->prioridade < args->paciencia) {
                    enfileirar(args->alta_prioridade, novoCliente);
                    printf("Cliente adicionado à fila de alta prioridade.\n");
                } else {
                    enfileirar(args->baixa_prioridade, novoCliente);
                    printf("Cliente adicionado à fila de baixa prioridade.\n");
                }
            }

        }

    }
}