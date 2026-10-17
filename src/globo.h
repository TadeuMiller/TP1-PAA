#ifndef GLOBO_H
#define GLOBO_H

#include "labirinto.h"

#define GLOBO_NUM_DIRECOES 4

int globo_vizinho(const Labirinto *lab, int linha, int coluna, int direcao,
                  int modo_globo, int *nova_linha, int *nova_coluna);

#endif
