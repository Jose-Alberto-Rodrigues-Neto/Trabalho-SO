#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include "fila.h"

// Criação da fila
Fila* cria_fila() {
    Fila *fila = (Fila*)malloc(sizeof(Fila));
    fila->front = NULL;
    fila->rear = NULL;
    fila->tamanho = 0;
    return fila;
}

// Verifica se a fila está vazia
int fila_vazia(Fila *fila) {
    return fila->tamanho == 0;
}

// Enfileirar um cliente na fila
void enfileirar(Fila* fila, Cliente* cliente) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->cliente = cliente;
    node->proximo = NULL;

    if (fila_vazia(fila)) {
        fila->front = node;
        fila->rear = node;
    } else {
        fila->rear->proximo = node;
        fila->rear = node;
    }
    fila->tamanho++;
}

// Desenfileirar um cliente da fila
Cliente* desenfileirar(Fila *fila) {
    if (fila_vazia(fila)) {
        return NULL;
    }

    Node *aux = fila->front;
    Cliente *cliente = aux->cliente;

    fila->front = fila->front->proximo;

    if (fila->front == NULL) {
        fila->rear = NULL;
    }

    free(aux);
    fila->tamanho--;

    return cliente;
}
