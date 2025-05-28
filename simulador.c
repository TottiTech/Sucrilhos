#include "simulador.h"
#include <stdio.h>
#include <stdlib.h>


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
    sim->memoria.frames = (int *)malloc(sizeof(int)* sim->memoria.num_frames);
    sim->memoria.tempo_carga = (int *)maloc(sizeof(int)*sim->memoria.num_frames);

    //Inicializar frames livres e com tempo 0
    for(int i = 0; i < sim->memoria.num_frames; i++){
        sim->memoria.frames[i] = -1;
        sim->memoria.tempo_carga[i] = 0;
    }

    //lista de processos zerada
    sim->num_processos = 0;
    sim->processos = NULL;

}