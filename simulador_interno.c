#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ENDERECO_INVALIDO -2
#define PAGE_FAULT -1

typedef struct Pagina{
    int presente;       // 1 se a pagina esta na mamoria, 0 caso contrario
    int frame;          // Numero do frame onde a pagina esta alocada (-1 se nao alocada)
    int modificada;     // 1 se a pagina foi modificada, 0 caso contrario
    int referenciada;   // 1 se a pagina foi referenciada, 0 caso o contrario
    int tempo_carga;    // Instante em que a pagina foi carregada na memoria
    int ultimo_acesso;  // Instante do ultimo acesso a pagina
} Pagina;

typedef struct Processo{
    int pid;                // Identificador do processo
    int tamanho;            // Tamanho do processo em bytes
    int num_paginas;        // Numero de processos
    Pagina* tabela_paginas; // Tabela de paginas do processo
} Processo;

typedef struct Frame{
    int pid;
    int pagina;
}Frame;

typedef struct MemoriaFisica{
    int num_frames;     // Numero total de frames em memoria fisica
    Frame *frames;        // Array de frames (cada elemento contem o pid e a pagina)
    int *tempo_carga;   // Tempo em que cada frame foi carregado (Para FIFO)
} MemoriaFisica;

typedef struct Simulador{
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

int substituir_pagina_fifo(Simulador *sim){
    int frame = -1;
    int menor_tempo = sim->tempo_atual+1;
    for(int i = 0; i < sim->memoria.num_frames; i++){
        if(sim->memoria.tempo_carga[i] < menor_tempo){
            menor_tempo = sim->memoria.tempo_carga[i];
            frame = i;
        }   
    }
    return frame;
}

int substituir_pagina_lru(Simulador *sim){
    int frame = -1;
    int menor_tempo = sim->tempo_atual+1;
    for(int i = 0; i < sim->memoria.num_frames; i++){
        int pid = sim->memoria.frames[i].pid;
        int pag = sim->memoria.frames[i].pagina;

        int tempo_acesso = sim->processos[pid].tabela_paginas[pag].ultimo_acesso;
        if(tempo_acesso < menor_tempo){
            menor_tempo = tempo_acesso;
            frame = i;
        }
    }
    return frame;
}

int traduzir_endereco(Simulador *sim, int pid, int endereco_virtual){
    int pagina = endereco_virtual / sim->tamanho_pagina;
    int deslocamento = endereco_virtual % sim->tamanho_pagina;
    if(pagina < 0 || pagina >= sim->processos[pid].num_paginas){
        return ENDERECO_INVALIDO;
    }
    Pagina *p = &sim->processos[pid].tabela_paginas[pagina];
    if(!p->presente){
        return PAGE_FAULT;
    }
    int frame = p->frame;
    return frame * sim->tamanho_pagina + deslocamento;
}

void extrair_pagina_deslocamento(Simulador *sim, int endereco_virtual, int *pagina, int *deslocamento){
    *pagina = endereco_virtual / sim->tamanho_pagina;
    *deslocamento = endereco_virtual % sim->tamanho_pagina;
}

int carregar_pagina(Simulador *sim, int pid, int pagina){
    int frame = -1;
    //verificar se há algum frame livre
    for(int i = 0; i < sim->memoria.num_frames; i++){
        if(sim->memoria.frames[i].pid == -1){
            frame = i;
            break;
        }
    }

    //caso não tenha frame livre, buscar qual frame sera substituido
    if(frame == -1){    
        if(sim->algoritmo == 0){
            frame = substituir_pagina_fifo(sim);
        }else{
            frame = substituir_pagina_lru(sim);
        }
        int pid_vitima = sim->memoria.frames[frame].pid; //carrega o pid da pagina que será substituda
        int pag_vitima = sim->memoria.frames[frame].pagina; // e a pagina que será substituida

        sim->processos[pid_vitima].tabela_paginas[pag_vitima].presente = 0; //altera de 1 (presente) pra 0 (não presente)
        sim->processos[pid_vitima].tabela_paginas[pag_vitima].frame = -1; // altera o frame atual da pagina pra -1 (em nenhum frame)

        printf("Page fault: Substituindo P%d-P%d por P%d-P%d no frame %d\n", 
               pid_vitima, pag_vitima, pid, pagina, frame);
    }

    //colocar pagina no frame
    sim->memoria.frames[frame].pid = pid;
    sim->memoria.frames[frame].pagina = pagina;
    sim->memoria.tempo_carga[frame] = sim->tempo_atual;

    //atualiza tabela de paginas
    Pagina *p = &sim->processos[pid].tabela_paginas[pagina];
    p->presente = 1;
    p->frame = frame;
    p->tempo_carga = sim->tempo_atual;
    p->ultimo_acesso = sim->tempo_atual;
    return 0;
}

int substituir_pagina_clock(Simulador *sim){
    //codigo a ser implementado
    return 0;
}

int substituir_pagina_random(Simulador *sim){
    //codigo a ser implementado
    return rand() % sim->memoria.num_frames;
}

int acessar_memoria(Simulador *sim, int pid, int endereco_virtual){
    sim->total_acessos++;
    
    int pagina = endereco_virtual / sim->tamanho_pagina;
    
    int end_fisico = traduzir_endereco(sim, pid, endereco_virtual);
    
    if(end_fisico == ENDERECO_INVALIDO){
        printf("Erro: endereço inválido para o processo %d.\n", pid);
        return -1;
    }
    
    if(end_fisico == PAGE_FAULT){
        // Page fault - carrega a página
        carregar_pagina(sim, pid, pagina);
        sim->page_faults++;
        end_fisico = traduzir_endereco(sim, pid, endereco_virtual);
    }
    
    // Atualiza o último acesso
    sim->processos[pid].tabela_paginas[pagina].ultimo_acesso = sim->tempo_atual;
    
    // Calcula o frame onde está a página
    int frame = sim->processos[pid].tabela_paginas[pagina].frame;
    
    // Exibe no formato solicitado
    printf("Tempo t=%d: Endereço Virtual (P%d): %d -> Página: %d -> Frame: %d -> Endereço Físico: %d\n",
           sim->tempo_atual, pid, endereco_virtual, pagina, frame, end_fisico);
    
    sim->tempo_atual++;
    
    return end_fisico;
}

// Função para simular um padrão de acesso sequencial
void simular_acesso_sequencial(Simulador *sim, int pid, int num_acessos) {
    printf("\n--- Simulando acesso sequencial para processo %d ---\n", pid);
    for(int i = 0; i < num_acessos && i < sim->processos[pid].tamanho; i += sim->tamanho_pagina/4) {
        acessar_memoria(sim, pid, i);
    }
}

// Função para simular um padrão de acesso aleatório
void simular_acesso_aleatorio(Simulador *sim, int pid, int num_acessos) {
    printf("\n--- Simulando acesso aleatório para processo %d ---\n", pid);
    for(int i = 0; i < num_acessos; i++) {
        int endereco = rand() % sim->processos[pid].tamanho;
        acessar_memoria(sim, pid, endereco);
    }
}

// Função para simular padrão de localidade temporal
void simular_localidade_temporal(Simulador *sim, int pid, int num_acessos) {
    printf("\n--- Simulando localidade temporal para processo %d ---\n", pid);
    int endereco_base = rand() % (sim->processos[pid].tamanho / 2);
    
    for(int i = 0; i < num_acessos; i++) {
        // 80% das vezes acessa próximo ao endereço base
        int endereco;
        if(rand() % 100 < 80) {
            endereco = endereco_base + (rand() % (sim->tamanho_pagina * 2));
            if(endereco >= sim->processos[pid].tamanho) {
                endereco = endereco_base;
            }
        } else {
            endereco = rand() % sim->processos[pid].tamanho;
        }
        acessar_memoria(sim, pid, endereco);
    }
}