#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

#define MAX_PIDS 10

sem_t *sem;

void processa_pids() {
    // Adquire o semáforo
    sem_wait(sem);

    FILE *file = fopen("LNG.txt", "r+");
    if (!file) {
        perror("Erro ao abrir o arquivo de PIDs");
        sem_post(sem); // Libera o semáforo antes de sair
        return;
    }

    int pids[MAX_PIDS];
    int count = 0;

    // Lê até 10 PIDs do arquivo
    while (fscanf(file, "%d", &pids[count]) == 1 && count < MAX_PIDS) {
        count++;
    }

    if (count == 0) {
        fclose(file);
        sem_post(sem); // Libera o semáforo antes de sair
        return; // Arquivo vazio
    }

    // Imprime os PIDs lidos
    printf("Analista: Imprimindo PIDs atendidos:\n");
    for (int i = 0; i < count; i++) {
        printf("%d\n", pids[i]);
    }

    // Remove os PIDs lidos do arquivo
    FILE *temp = fopen("LNG.txt", "w");
    if (!temp) {
        perror("Erro ao criar arquivo temporário");
        fclose(file);
        sem_post(sem); // Libera o semáforo antes de sair
        return;
    }

    // Reabre o arquivo original para leitura
    fseek(file, 0, SEEK_SET);
    int pid;
    while (fscanf(file, "%d", &pid) == 1) {
        fprintf(temp, "%d\n", pid);
    }

    fclose(file);
    fclose(temp);

    // Libera o semáforo
    sem_post(sem);
}

int main() {
    // Cria o semáforo
    sem = sem_open("/sem_block", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("Erro ao criar semáforo");
        return 1;
    }

    printf("Aguardando o semáforo ser liberado...\n");

    // Aguarda a liberação do semáforo para executar o processamento
    while (1) {
        // Aguarda o semáforo ser liberado antes de prosseguir
        sem_wait(sem);
        printf("Semáforo liberado! Processando PIDs...\n");
        processa_pids();
    }

    // Fecha o semáforo
    sem_close(sem);
    sem_unlink("/sem_block");

    return 0;
}
