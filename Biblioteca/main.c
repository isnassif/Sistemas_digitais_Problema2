#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "./hps_0.h"
#include "api.h"  // <-- Agora só precisa incluir isso!

#define IMAGE_PATH "/home/aluno/TEC499/TP02/SirioeGuerra/imagem.mif"

void exibirMenu() {
    printf("\n=== Escolha o Algoritmo de Processamento ===\n");
    printf("1 - Reset\n");
    printf("2 - Replicação 2x\n");
    printf("3 - Decimação 2x\n");
    printf("4 - Zoom NN 2x\n");
    printf("5 - Média 2x\n");
    printf("6 - Cópia Direta\n");
    printf("7 - Replicação 4x\n");
    printf("8 - Decimação 4x\n");
    printf("9 - Zoom NN 4x\n");
    printf("10 - Média 4x\n");
    printf("0 - Sair\n");
    printf("Selecione uma opção: ");
}

int main() {
    // Inicializa os valores base (do hps_0.h)
    IMAGE_MEM_BASE_VAL = ONCHIP_MEMORY2_1_BASE;
    CONTROL_PIO_BASE_VAL = PIO_LED_BASE;
    
    // Carrega a imagem MIF usando a função assembly
    int bytes = carregarImagemMIF(IMAGE_PATH);
    if (bytes < 0) {
        perror("Erro ao carregar imagem");
        return 1;
    }
        
    // Mapeia a ponte lightweight usando a função assembly
    if (mapearPonte() < 0) {
        perror("Erro ao mapear ponte lightweight");
        limparRecursos();
        return 1;
    }
    
    // Transfere a imagem para o FPGA usando a função assembly
    transferirImagemFPGA(bytes);
    
    // Loop do menu
    int opcao = -1;
    while (opcao != 0) {
        exibirMenu();
        
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n'); // Limpa buffer
            printf("Entrada inválida!\n");
            continue;
        }
        
        if (opcao == 0) {
            printf("Encerrando...\n");
            break;
        }
        
        // Obtém o código do estado usando a função assembly
        int codigo = obterCodigoEstado(opcao);
        if (codigo < 0) {
            printf("Opção inválida!\n");
            continue;
        }
        
        // Envia comando para o FPGA usando a função assembly
        enviarComando(codigo);
        
    }
    
    // Limpeza final usando a função assembly
    limparRecursos();
    printf("Sistema encerrado!\n");
    
    return 0;
}
