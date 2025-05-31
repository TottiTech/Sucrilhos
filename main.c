#include <stdio.h>
#include "simulador_interno.c"
#include "simulador_container.c"

int main(){
    Simulador sim;

    //Solicita configurações iniciais do simulador
    int tam_p, tam_m, alg, temp;
    do{
        tam_p = -1;
        tam_m = -1;
        alg = -1;
        printf("Digite o tamanho da página em bytes: ");
        scanf("%d", &temp);
        if(temp <= 0){
            printf("Tamanho da página não pode ser 0 ou negativo.\n");
            continue;
        }else{tam_p = temp;}

        printf("Digite o tamanho da memória física em bytes: ");
        scanf("%d", &temp);
        if(temp < tam_p){
            printf("Memória não pode ser menor que o tamanho da página (mínimo 1 frame).\n");
            continue;
        }else{tam_m = temp;}

        printf("0 = FIFO, 1 = LRU, 2 = CLOCK, 3 = RANDOM\n");
        printf("Escolha o algoritmo de substituição: ");
        scanf("%d", &temp);
        if(temp < 0 || temp > 3){
            printf("Algoritmo inválido, escolha entre 0, 1, 2 ou 3.\n");
            continue;
        }else if(temp == 1 || temp == 2 || temp == 3){
            printf("Algoritmo ainda não implementado, executando com algoritmo padrão (FIFO)\n");
            alg = 0;
        }else{alg = temp;}
    }while(tam_p == -1|| tam_m == -1||alg == -1);

    iniciar_simulador(&sim, tam_p, tam_m, alg);

    int opt;
    do{
        printf("\n=== MENU ===\n");
        printf("1. Adicionar processo\n");
        printf("2. Executar simulação\n");
        printf("3. Exibir estatísticas\n");
        printf("4. Exibir estado da memória\n");
        printf("5. Limpar simulador\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opt);

        if(opt == 1){
            int tam;
            printf("Tamanho do processo em bytes: ");
            scanf("%d", &tam);
            if(tam > 0){
                criar_processo(&sim, tam);
            }else{
                printf("Impossível criar processo com tamanho 0 ou negativo.\n");
            }

        }
        else if(opt == 2){
            if(sim.num_processos > 0){
                exec_sim(&sim);
            }else{
                printf("Não é possível executar a simulação sem processos.\n");
            }
        }else if(opt == 3){
            exibir_stats(&sim);
        }else if(opt == 4){
            exibir_memoria(&sim);
        }else if(opt == 5){
            // Limpar todos os processos e resetar estatísticas
            encerrar_simulador(&sim);
            iniciar_simulador(&sim, sim.tamanho_pagina, sim.tamanho_memoria_fisica, sim.algoritmo);
            printf("Simulador limpo e reinicializado.\n");
        }
        

    }while(opt != 0);
    
    encerrar_simulador(&sim);
    printf("Simulador encerrado.\n");
    return 0;
}