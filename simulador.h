#ifndef SIMULADOR_H
#define SIMULADOR_H
#define ENDERECO_INVALIDO -2
#define PAGE_FAULT -1

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

int traduzir_endereco(Simulador *sim, int pid, int endereco_virtual){
    int pagina = endereco_virtual / sim->tamanho_pagina;
    int deslocamento = endereco_virtual % sim->tamanho_pagina;
    if(pagina < 0 || pagina > sim->processos[pid].num_paginas){
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
    //codigo
}

int verificar_pagina_presente(Simulador *sim, int pid, int pagina){
    //codigo
    return 0;
}

int carregar_pagina(Simulador *sim, int pid, int pagina){
    //codigo
    return 0;
}

int substituir_pagina_fifo(Simulador *sim){
    //codigo
    return 0;
}

int substituir_pagina_lru(Simulador *sim){
    //codigo
    return 0;
}

int substituir_pagina_clock(Simulador *sim){
    //codigo
    return 0;
}

int substituir_pagina_random(Simulador *sim){
    //codigo
    return 0;
}



void registrar_acesso(Simulador *sim, int pid, int pagina){
    //registra o tempo em que a pagina foi acessada (usado em LRU)
    sim->processos[pid].tabela_paginas[pagina].ultimo_acesso = sim->tempo_atual;
}

int acessar_memoria(Simulador *sim, int pid, int endereco_virtual){
    sim->total_acessos++;
    sim->tempo_atual++;
    int pag = endereco_virtual / sim->tamanho_pagina;

    int end_fisico = traduzir_endereco(sim, pid, endereco_virtual);
    if(end_fisico == ENDERECO_INVALIDO){
        printf("Erro: endereço invalido para o processo.");
        return -1;
    }
    if(end_fisico == PAGE_FAULT){
        carregar_pagina(sim, pid, pag);
        sim->page_faults++;
        end_fisico = traduzir_endereco(sim, pid, endereco_virtual);

    }
    sim->processos[pid].tabela_paginas[pag].ultimo_acesso = sim->tempo_atual;

    return end_fisico;
}

#endif