# Trabalho-S.O.

Uma aplicação onde será utilizado ao menos duas threads para simular um serviço de atendimento com uma `Recepção` que irá criar um novo processo `Cliente` (inicialmente dormente), uma `Fila escalonada` que vai armazenar o `id` do `Cliente` e um `Atendente` que vai escrever o número do cliente em um novo arquivo e `calcular sua avaliação`, no fim matando o processo do `Cliente`.

## Imagem com as propriedades definidas pelo professor em sala

![image](https://github.com/user-attachments/assets/96124180-73f6-4c87-8650-e9a2bfb2048c)

Obs: todas as informações a seguir são baseadas no esquema mostrado em sala pelo professor

## Limitações do Trabalho

- Linux
- C

# Processo de Atendimento

## Thread 1 - `Recepção`

### Entradas

- [ ] Número máximo de clientes
- [ ] Prioridade baixa

### Funcionalidades

- [ ] Criar processo `Cliente` (dentro de um laço)
- [ ] Adicionar valor de `prioridade aleatória` (podendo ser baixa, ou alta) ao `Cliente` 
- [ ] Prioridade alta = prioridade baixa / 2
- [ ] Atribuir `Hora de Chegada`, valor em `milisegundos` que conta desde o começo do programa
- [ ] Adiciona `Cliente` na fila

## Thread 2 - `Atendente`

### Funcionalidades

- [ ] Lê na fila o próximo `Cliente` a ser atendido, conforme foi ordenado na fila
- [ ] Liga o processo do `Cliente`
- [ ] Escreve o `id` do `Cliente` (encontrado no arquivo que o processo `Cliente` criou) em um novo arquivo que irá gerar uma lista de ids dos clientes atendidos
- [ ] Calcula se o `Cliente` está satisfeito levando em conta o tempo de atendimento (tempo de atendimento = horário atendido - hora de chegada) e a prioridade do `Cliente`, ou seja, se o cliente conseguiu ser atendido dentro do tempo de espera estimado (prioridade - tempo de atendimento)
- [ ] Mata o processo `Cliente` atendido com o PID guardado na `Fila escalonada`
- [ ] Tira o `Cliente` da fila e passa para o próximo
- [ ] Quando a fila for lida por inteiro, ativa o processo `Analista`, que irá avaliar o código

## Thread 3 - `Fila Escalonada` (Você pode escolher se quer fazer com ou sem thread)

### Funcionalidades

- [ ] Tamanho da fila: Tamanho do máximo de clientes
- [ ] Adicionar `Clientes`, guardando seus PIDs
- [ ] Ordenar `Clientes` utilizando a ordem de chegada e sua "prioridade"

# Processo Cliente

### Funcionalidades

- [ ] Gera um `id` aleatório
- [ ] Escreve esse `id` em um arquivo de texto

# Processo Analista

Será gerado pelo professor.

---

# Método de correção

- Baseado na taxa de satisfação do cliente
- Tempo de execução total
