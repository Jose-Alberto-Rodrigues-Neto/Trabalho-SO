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

void satisfacao_cliente(long tempo_de_espera, long paciencia, pid_t pid) {
    if (tempo_de_espera <= paciencia) {
        printf("Cliente PID=%d satisfeito\n", pid);
    } else {
        printf("Cliente PID=%d insatisfeito\n", pid);
    }
}


void *atendente(void* arg) {
    printf("ATENDENTE INICIADO\n");

    AtendenteArgs* args = (AtendenteArgs*)arg;
    Fila* clientes;
    int fila_aux = 1;

    while (1) {
        clientes = (fila_aux == 1) ? args->fila_alta : args->fila_baixa;

        while (fila_vazia(clientes)) {
            usleep(100000); // Espera até que a fila tenha clientes.
        }

        sem_wait(args->sem_atend);
        Cliente* cliente = desenfileirar(clientes);
        sem_post(args->sem_atend);

        if (!cliente) continue;

        printf("Atendendo cliente PID=%d\n", cliente->pid);

        struct timeval tv;
        gettimeofday(&tv, NULL);
        long wait_time = tv.tv_usec - cliente->chegada;

        satisfacao_cliente(wait_time, cliente->prioridade, cliente->pid);

        free(cliente);

        fila_aux = (fila_aux == 1) ? 0 : 1;
    }

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