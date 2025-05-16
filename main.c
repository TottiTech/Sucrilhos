#include <stdio.h>

typedef struct{
    int presente;       // 1 se a pagina esta na mamoria, 0 caso contrario
    int frame;          // Numero do frame onde a pagina esta alocada (-1 se nao alocada)
    int modificada;     // 1 se a pagina foi modificada, 0 caso contrario
    int referenciada;   // 1 se a pagina foi referenciada, 0 caso o contrario
    int tempo_carga;    // Instante em que a pagina foi carregada na memoria
    int ultimo_acesso;  // Instante do ultimo acesso a pagina
} Pagina;

typedef struct{
    int pid;                // Identificador do processo
    int tamanho;            // Tamanho do processo em bytes
    int num_paginas;        // Numero de processos
    Pagina* tabela_paginas; // Tabela de paginas do processo
} Processo;

typedef struct{
    int num_frames;     // Numero total de frames em memoria fisica
    int *frames;        // Array de frames (cada elemento contem o pid e a pagina)
                        // Ex: frames[1] = (pid << 16) | num_pagina
    int *tempo_carga;   // Tempo em que cada frame foi carregado (Para FIFO)
} MemoriaFisica;

typedef struct{
    int tempo_atual;            // Contador de tempo de simulacao
    int tamanho_pagina;         // Tamanho da pagina em bytes
    int tamanho_memoria_fisica; // Tamanho da memoria fisica em bytes
    int num_processos;          // Numero de processos na simulacao
    Processo *processos;        // Array de processos
    MemoriaFisica memoria;      // Memoria fisica

    // Estatisticas
    int total_acessos;  // Total de acessos a memoria
    int page_faults;    // Total de page faults ocorridos

                        // Algoritmos de substituicao atual
    int algoritmo;      // 0=FIFO, 1=LRU, 2=CLOCK, 3=RANDOM, 4=CUSTOM
} Simulador;

int main(){

    return 0;
}