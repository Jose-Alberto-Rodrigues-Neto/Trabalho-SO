#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include "fila.h"

typedef struct {
        int pid;
        long int chegada;
        int prioridade;
        int tempo_atendimento;
} Cliente;

typedef struct {
        Cliente cliente;
        struct Node *proximo;
} Node;

typedef struct {
        Node *front;
        Node *rear;
        int tamanho;
} Fila;

Fila* cria_fila() {
        Fila *fila = (Fila*)malloc(sizeof(Fila));

        fila->front = NULL;
        fila->rear = NULL;
        fila->tamanho = 0;

        return fila;
}

int fila_vazia(Fila *fila) {
        return fila->tamanho == 0;
}

void enfileirar(Fila *fila, Cliente cliente) {
        Node *node = (Node*)malloc(sizeof(Node));
        node->cliente = cliente;
        node->proximo = NULL;

        if(esta_vazia(fila)) {
                fila->front = node;
                fila->rear = node;
        } else {
                fila->rear->proximo = node;
                fila->rear = node;
        }
        fila->tamanho++;
}

void desenfileirar(Fila *fila) {
        if(esta_vazia(fila)) {
                return NULL;
        }

        Node *aux = fila->front;
        Cliente cliente = temp->cliente;

        fila->front = fila->front->proximo;

        if(fila->front == NULL) {
                fila->rear == NULL;
        }

        free(temp);
        fila->tamanho--;

        return cliente;
}
