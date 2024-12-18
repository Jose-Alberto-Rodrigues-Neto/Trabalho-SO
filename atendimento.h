#ifndef ATENDIMENTO_H
#define ATENDIMENTO_H

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

// Estrutura dos argumentos passados para a thread recepcao
typedef struct {
    int num_clientes;
    int paciencia;
    long tempo_chegada;
    Fila* baixa_prioridade;
    Fila* alta_prioridade;
} RecepcaoArgs;

// Estrutura dos argumentos passados para a thread atendente
typedef struct {
    int num_clientes;
    Fila* fila;
} AtendenteArgs;

// Função do atendente
void* atendente(void* args);

// Função para iniciar a recepção
void* recepcao(void* arg);

#endif // ATENDIMENTO_H
