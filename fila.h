#ifndef FILA_H
#define FILA_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>

// Estrutura do cliente
typedef struct {
    int pid;
    long int chegada;
    int prioridade;
    int tempo_atendimento;
} Cliente;

// Nó da fila
typedef struct Node {
    Cliente cliente;
    struct Node *proximo;
} Node;

// Estrutura da fila
typedef struct {
    Node *front;
    Node *rear;
    int tamanho;
} Fila;

// Funções da fila
Fila* cria_fila();
int fila_vazia(Fila *fila);
void enfileirar(Fila *fila, Cliente cliente);
Cliente desenfileirar(Fila *fila);

#endif // FILA_H
