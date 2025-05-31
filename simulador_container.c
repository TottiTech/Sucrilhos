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
    
    // Inicializar gerador de números aleatórios
    srand(time(NULL));
    
    printf("Simulador inicializado:\n");
    printf("- Tamanho da página: %d bytes\n", tam_pagina);
    printf("- Memória física: %d bytes (%d frames)\n", tam_memoria, sim->memoria.num_frames);
    printf("- Algoritmo: %s\n", algoritmo == 0 ? "FIFO" : "LRU");
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

    printf("Processo %d criado com %d páginas (tamanho: %d bytes)\n", 
           novo_processo.pid, novo_processo.num_paginas, tam);
}

//representação da memoria fisica
void exibir_memoria(Simulador *sim){
    printf("\n=== Estado da Memória Física ===\n");
    printf("Frames totais: %d\n", sim->memoria.num_frames);
    
    int frames_ocupados = 0;
    for(int i = 0; i < sim->memoria.num_frames; i++){
        printf("Frame %2d: ", i);
        Frame f = sim->memoria.frames[i];
        if(f.pid == -1){
            printf("[ LIVRE ]\n");
        }else{
            printf("[ P%d-Pág%d ] (carregado em t=%d)\n", 
                   f.pid, f.pagina, sim->memoria.tempo_carga[i]);
            frames_ocupados++;
        }
    }
    printf("Ocupação: %d/%d frames (%.1f%%)\n", 
           frames_ocupados, sim->memoria.num_frames, 
           (100.0 * frames_ocupados) / sim->memoria.num_frames);
    printf("===============================\n");
}

void exibir_stats(Simulador *sim){
    printf("\n=== Estatísticas da Simulação ===\n");
    printf("Tempo atual: %d\n", sim->tempo_atual);
    printf("Total de acessos: %d\n", sim->total_acessos);
    printf("Total de page faults: %d\n", sim->page_faults);
    if(sim->total_acessos > 0){
        double taxa_fault = (100.0 * sim->page_faults) / sim->total_acessos;
        double taxa_hit = 100.0 - taxa_fault;
        printf("Taxa de page faults: %.2f%%\n", taxa_fault);
        printf("Taxa de hits: %.2f%%\n", taxa_hit);
    }
    printf("Algoritmo utilizado: %s\n", 
           sim->algoritmo == 0 ? "FIFO" : "LRU");
    printf("===============================\n");
}

void exec_sim(Simulador *sim){
    printf("\n=== Iniciando Simulação ===\n");
    
    if(sim->num_processos == 0) {
        printf("Erro: Nenhum processo para simular.\n");
        return;
    }
    
    printf("Processos disponíveis:\n");
    for(int i = 0; i < sim->num_processos; i++) {
        printf("- Processo %d: %d bytes (%d páginas)\n", 
               sim->processos[i].pid, 
               sim->processos[i].tamanho,
               sim->processos[i].num_paginas);
    }
    
    // Resetar estatísticas para nova simulação
    sim->tempo_atual = 0;
    sim->total_acessos = 0;
    sim->page_faults = 0;
    
    // Limpar memória física
    for(int i = 0; i < sim->memoria.num_frames; i++){
        sim->memoria.frames[i].pid = -1;
        sim->memoria.frames[i].pagina = -1;
        sim->memoria.tempo_carga[i] = 0;
    }
    
    // Limpar tabelas de páginas
    for(int i = 0; i < sim->num_processos; i++) {
        for(int j = 0; j < sim->processos[i].num_paginas; j++) {
            sim->processos[i].tabela_paginas[j].presente = 0;
            sim->processos[i].tabela_paginas[j].frame = -1;
            sim->processos[i].tabela_paginas[j].ultimo_acesso = 0;
            sim->processos[i].tabela_paginas[j].tempo_carga = 0;
        }
    }
    
    printf("\nMemória física limpa. Iniciando padrões de acesso...\n");
    
    // Teste específico para garantir páginas diferentes
    if(sim->num_processos > 0) {
        printf("\n--- Teste de páginas diferentes ---\n");
        
        // Acessar primeira página
        acessar_memoria(sim, 0, 0);
        
        // Acessar segunda página se existir
        if(sim->processos[0].tamanho > sim->tamanho_pagina) {
            acessar_memoria(sim, 0, sim->tamanho_pagina);
        }
        
        // Acessar terceira página se existir
        if(sim->processos[0].tamanho > 2 * sim->tamanho_pagina) {
            acessar_memoria(sim, 0, 2 * sim->tamanho_pagina);
        }
        
        // Acessar última página
        int endereco_final = sim->processos[0].tamanho - 1;
        acessar_memoria(sim, 0, endereco_final);
    }
    
    // Simular diferentes padrões de acesso para cada processo
    for(int i = 0; i < sim->num_processos; i++) {
        int pid = sim->processos[i].pid;
        int num_acessos = 5;
        
        // Padrão 1: Acesso sequencial
        simular_acesso_sequencial(sim, pid, num_acessos);
        
        // Padrão 2: Acesso aleatório
        simular_acesso_aleatorio(sim, pid, num_acessos);
        
        // Padrão 3: Localidade temporal
        simular_localidade_temporal(sim, pid, num_acessos);
        
        printf("\n");
    }
    
    // Simular competição entre processos
    if(sim->num_processos > 1) {
        printf("\n--- Simulando competição entre processos ---\n");
        for(int round = 0; round < 10; round++) {
            int pid = rand() % sim->num_processos;
            int endereco = rand() % sim->processos[pid].tamanho;
            acessar_memoria(sim, pid, endereco);
        }
    }
    
    printf("\n=== Simulação Concluída ===\n");
    exibir_stats(sim);
}