#include "simulador.h"
#include <stdio.h>

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
        printf("Memória não pode ser 0 ou negativa.\n");
        continue;
    }else{tam_p = temp;}

    printf("Digite o tamanho da memória física em bytes: ");
    scanf("%d", &temp);
    if(temp < tam_p){
        printf("Memoria não pode ser menor que a página (1 frame).\n");
        continue;
    }else{tam_m = temp;}

    printf("0 = FIFO, 1 = LRU, 2 = CLOCK, 3 = RANDOM\n");
    printf("Escolha o algoritmo de substituição: ");
    scanf("%d", &temp);
    if(temp < 0 || temp > 3){
        printf("Algoritmo invalido, escolha entre 0, 1, 2 ou 3.\n");
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
        printf("1.Adicionar processo\n");
        printf("2.Executar simulação\n");
        printf("3.Exibir Estatisticas\n");
        printf("4.Exibir estado final da memória\n");
        printf("0.\n");
        printf("Escolha: \n");
        scanf("%d", &opt);

        if(opt == 1){
            int tam;
            printf("Tamanho do processo em bytes: ");
            scanf("%d", &tam);
        }
        else if(opt == 2){
            if(sim.num_processos > 0){
                exec_sim(&sim);
            }else{
                printf("Não é possível executar a simulação sem processos\n");
            }
        }else if(opt == 3){
            exibir_stats(&sim);
        }else if(opt == 4){
            exibir_memoria(&sim);
        }
        

    }while(opt != 0);
    
    return 0;
}