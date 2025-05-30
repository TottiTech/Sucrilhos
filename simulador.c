#include "simulador.h"
#include <stdio.h>
#include <stdlib.h>


//inicializa um simulador com paramatros passados
void iniciar_simulador(Simulador *sim, int tam_pagina, int tam_memoria, int algoritmo){
    //Inicialização do simulador com os parametros passados
    sim->tamanho_pagina = tam_pagina;
    sim->tamanho_memoria_fisica = tam_memoria;
    sim->tempo_atual = 0;
    sim->total_acessos = 0;
    sim->page_faults = 0;
    sim->algoritmo = algoritmo;

    //Calcular frames
    sim->memoria.num_frames = tam_memoria/tam_pagina;
    sim->memoria.frames = (Frame *)malloc(sizeof(Frame)* sim->memoria.num_frames);
    sim->memoria.tempo_carga = (int *)malloc(sizeof(int)*sim->memoria.num_frames);

    //Inicializar frames livres e com tempo 0
    for(int i = 0; i < sim->memoria.num_frames; i++){
        sim->memoria.frames[i].pid = -1;
        sim->memoria.frames[i].pagina = -1;
        sim->memoria.tempo_carga[i] = 0;
    }

    //lista de processos zerada
    sim->num_processos = 0;
    sim->processos = NULL;
}

void encerrar_simulador(Simulador *sim){
    //liberar memoria da memoria física
    free(sim->memoria.frames);
    free(sim->memoria.tempo_carga);

    //liberar memoria da tabela de paginas
    for(int i = 0; i < sim->num_processos; i++){
        free(sim->processos[i].tabela_paginas);
    }

    //Libera memoria dos processos
    free(sim->processos);

}

//criar processo e adicina-lo ao simulador
void criar_processo(Simulador *sim, int tam){
    //inicializar um novo processo ('cacular pid, numero de paginas, alocar tabela de paginas, etc)
    Processo novo_processo;
    novo_processo.pid = sim->num_processos; // pid = indice
    novo_processo.tamanho = tam;
    novo_processo.num_paginas = (tam + sim->tamanho_pagina-1)/sim->tamanho_pagina; //divisao pra garantir que o processo todo caiba nas pags
    novo_processo.tabela_paginas = (Pagina *) malloc(sizeof(Pagina)*novo_processo.num_paginas);

    //inicializar tabela de paginas
    for(int i = 0; i < novo_processo.num_paginas; i++){
        novo_processo.tabela_paginas[i].presente = 0;
        novo_processo.tabela_paginas[i].frame = -1;
        novo_processo.tabela_paginas[i].modificada = 0;
        novo_processo.tabela_paginas[i].referenciada = 0;
        novo_processo.tabela_paginas[i].tempo_carga = 0;
        novo_processo.tabela_paginas[i].ultimo_acesso = 0;
    }

    //alocar o processo no simulador
    sim->processos = (Processo *)realloc(sim->processos, sizeof(Processo) * (sim->num_processos +1 ));
    sim->processos[sim->num_processos] = novo_processo;
    sim->num_processos++;

    printf("Processo %d criado com %d paginas\n", novo_processo.pid, novo_processo.num_paginas);
}

//representação da memoria fisica
void exibir_memoria(Simulador *sim){
    printf("\nEstado da Memória Física:\n");
    for(int i = 0; i < sim->memoria.num_frames; i++){
        printf("--------\n");

        Frame f = sim->memoria.frames[i];
        if(f.pid == -1){
            printf("| ---- |\n"); //frame vazio
        }else{
            printf("| P%-2d-%-2d |\n", f.pid, f.pagina);
        }
    }
    printf("--------\n");
}

void exibir_stats(Simulador *sim){
    printf("\n=== Estatisticas ===\n");
    printf("Total de acessos: %d\n", sim->total_acessos);
    printf("Total de page faults: %d\n", sim->page_faults);
    if(sim->total_acessos > 0){
        double taxa = (100.0 * sim->page_faults) / sim->total_acessos;
        printf("Taxa de page faults: %.2f%%\n", taxa);
    }
    printf("=====================\n");
}

void exec_sim(Simulador *sim){
    printf("Executando simulação");
    //codigo
    printf("Simulação executada");
}