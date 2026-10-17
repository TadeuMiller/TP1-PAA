#ifndef BUSCA_H
#define BUSCA_H

#include "labirinto.h"

typedef struct {
    Posicao *posicoes;
    size_t tamanho;
} Caminho;

typedef struct {
    Caminho *caminhos;
    size_t quantidade;
    size_t chamadas_recursivas;
    size_t nivel_maximo;
} ResultadoBusca;

/* Retorna 1 se houver solucao, 0 se nao houver e -1 em caso de erro. */
int busca_resolver(const Labirinto *labirinto, int todos_caminhos,
                   ResultadoBusca *resultado);

void busca_resultado_liberar(ResultadoBusca *resultado);

#endif
