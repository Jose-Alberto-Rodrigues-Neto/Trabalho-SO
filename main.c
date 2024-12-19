#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "fila.h"
#include "atendimento.h"

void print_fila(Fila* fila) {
    if (fila_vazia(fila)) {
        printf("Fila está vazia.\n");
        return;
    }

    Node* atual = fila->front;
    printf("Fila (tamanho: %d):\n", fila->tamanho);
    while (atual != NULL) {
        Cliente* cliente = atual->cliente;
        printf("Cliente PID: %d, Prioridade: %d, Chegada: %ld\n", 
               cliente->pid, cliente->prioridade, cliente->chegada);
        atual = atual->proximo;
    }
    printf("\n");
}

int main(int argc, char **argv){

    if (argc < 3) {
        fprintf(stderr, "Uso: %s <num_clientes> <paciencia>\n", argv[0]);
        return EXIT_FAILURE;
    }
    //pegando entradas do usuário
    int num_clientes = atoi(argv[1]);
    int paciencia = atoi(argv[2]);

    //semáforos
    sem_t sem_atend_local, sem_block_local;
    sem_init(&sem_atend_local, 0, 1);
    sem_init(&sem_block_local, 0, 1);
    
    //criando threads
    pthread_t recepcao_thread, atendente_thread;

    //fila
    Fila* baixa_prioridade;
    Fila* alta_prioridade;

    alta_prioridade = cria_fila();
    baixa_prioridade = cria_fila();

    if (alta_prioridade == NULL || baixa_prioridade == NULL) {
        fprintf(stderr, "Erro ao inicializar as filas.\n");
        return EXIT_FAILURE;
    }

    //inicializando randomizador
    srand(time(NULL));

    //inicializando timer
    clock_t tempo_ini, tempo_fim;

    tempo_ini = clock();

    //iniciando argumentos da recepção
    RecepcaoArgs* recepcao_args = malloc(sizeof(RecepcaoArgs));
    recepcao_args->num_clientes = num_clientes;
    recepcao_args->paciencia = paciencia;
    recepcao_args->tempo_chegada = tempo_fim;
    recepcao_args->baixa_prioridade = baixa_prioridade;
    recepcao_args->alta_prioridade = alta_prioridade;
    recepcao_args->sem_atend = &sem_atend_local;
    recepcao_args->sem_block = &sem_block_local;

    //iniciando argumentos do atendente, alta prioridade
    AtendenteArgs* atendenteArgs = malloc(sizeof(AtendenteArgs));
    atendenteArgs->num_clientes = num_clientes;
    atendenteArgs->fila_alta = alta_prioridade;
    atendenteArgs->fila_baixa = baixa_prioridade;
    atendenteArgs->sem_atend = &sem_atend_local;
    atendenteArgs->sem_block = &sem_block_local;

    pthread_create(&recepcao_thread, NULL, recepcao, recepcao_args);
    sleep(1); //espera 2 segundos antes de ligar atendente
    printf("\nSLEEP DESLIGADO\n");
    pthread_create(&atendente_thread, NULL, atendente, atendenteArgs);

    pthread_join(recepcao_thread, NULL);
    pthread_join(atendente_thread, NULL);
    // Imprimir filas ao final
    printf("Fila de alta prioridade:\n");
    print_fila(alta_prioridade);

    printf("Fila de baixa prioridade:\n");
    print_fila(baixa_prioridade);

    // Após uso
    sem_destroy(&sem_atend_local);
    sem_destroy(&sem_block_local);

    free(recepcao_args);
    free(atendenteArgs);
    free(baixa_prioridade);
    free(alta_prioridade);

    return 0;
}