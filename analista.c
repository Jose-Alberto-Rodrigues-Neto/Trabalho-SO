#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX_PIDS 10

sem_t *sem;

void processa_pids() {
    printf("\nAnalista: Processando PIDs...\n");

    FILE *file = fopen("LNG.txt", "r+");
    if (!file) {
        perror("Erro ao abrir o arquivo de PIDs");
        return;
    }

    int pids[MAX_PIDS];
    int count = 0;

    // Lê os PIDs do arquivo
    while (fscanf(file, "%d", &pids[count]) == 1 && count < MAX_PIDS) {
        count++;
    }

    if (count == 0) {
        printf("Nenhum PID para processar.\n");
        fclose(file);
        return;
    }

    printf("Analista: PIDs processados:\n");
    for (int i = 0; i < count; i++) {
        printf("PID=%d\n", pids[i]);
    }

    // Limpa o conteúdo do arquivo
    freopen("LNG.txt", "w", file);
    fclose(file);
}

int main() {
    // Abre o semáforo
    sem = sem_open("/sem_block", O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("Erro ao abrir semáforo");
        return 1;
    }

    printf("Analista aguardando o semáforo...\n");

    // Aguarda a liberação do semáforo
    while (1) {
        sem_wait(sem);
        processa_pids();
    }

    // Fecha o semáforo
    sem_close(sem);
    sem_unlink("/sem_block");

    return 0;
}
