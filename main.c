#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "fila.h"
#include "atendimento.h"

int main(int argc, char **argv){
    //pegando entradas do usuário
    int num_clientes = atoi(argv[1]);
    int paciencia = atoi(argv[2]);

    //semáforos
        sem_t *sem_atend, *sem_block;
    
    //criando threads
    pthread_t recepcao_thread, atendente_baixa_prioridade_thread, atendente_alta_prioridade_thread;

    //fila
    Fila* baixa_prioridade;
    Fila* alta_prioridade;

    alta_prioridade = cria_fila();
    baixa_prioridade = cria_fila();

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

    //iniciando argumentos do atendente, baixa prioridade
    AtendenteArgs* atendente_baixa_prioridade = malloc(sizeof(AtendenteArgs));
    atendente_baixa_prioridade->num_clientes = num_clientes;
    atendente_baixa_prioridade->fila = baixa_prioridade;
    atendente_baixa_prioridade->sem_atend = sem_atend;
    atendente_baixa_prioridade->sem_block = sem_block;

    //iniciando argumentos do atendente, alta prioridade
    AtendenteArgs* atendente_alta_prioridade = malloc(sizeof(AtendenteArgs));
    atendente_alta_prioridade->num_clientes = num_clientes;
    atendente_alta_prioridade->fila = alta_prioridade;
    atendente_alta_prioridade->sem_atend = sem_atend;
    atendente_alta_prioridade->sem_block = sem_block;

    pthread_create(&recepcao_thread, NULL, recepcao, recepcao_args);
    pthread_create(&atendente_alta_prioridade_thread, NULL, atendente, atendente_alta_prioridade);
    pthread_create(&atendente_baixa_prioridade_thread, NULL, atendente, atendente_baixa_prioridade);

    pthread_join(recepcao_thread, NULL);
    pthread_join(atendente_alta_prioridade_thread, NULL);
    pthread_join(atendente_baixa_prioridade_thread, NULL);

    return 0;
}