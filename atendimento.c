#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

//semaforos
#include <semaphore.h>
#include <fcntl.h>

// sem_open("/sem_atend", O_CREAT | O_EXCL, 0644, 1);

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

sem_t *sem_atend, *sem_block;

Fila *clientes;

void *atendente(void *args) {
        struct timeval tv;
        long int wait_time;
        Cliente cliente = clientes->front->cliente;

        pid_t cpid = cliente->pid;
        kill(cpid, SIGCONT);

        sem_wait(sem_atend);
        sem_wait(sem_block);
        sem_close(sem_block);

        FILE *lng = fopen("lng.txt", "a");
        fprintf(lng, "%d\n", cpid);
        fclose(lng);

        sem_post(sem_block);
        if(gettimeofday(&tv, NULL) != 0) {
                perror("erro no gettimeofday\n");
                return 1;
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