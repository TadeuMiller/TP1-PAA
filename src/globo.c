#include "globo.h"

int globo_vizinho(const Labirinto *lab, int linha, int coluna, int direcao,
                  int modo_globo, int *nova_linha, int *nova_coluna) {
    int linhas, colunas;
    int l = linha;
    int c = coluna;

    if (lab == NULL || nova_linha == NULL || nova_coluna == NULL) {
        return 0;
    }
    linhas = (int)lab->linhas;
    colunas = (int)lab->colunas;

    //Anda uma casa na direcao pedida
    if (direcao == 0) {
        l = linha - 1;      
    } else if (direcao == 1) {
        c = coluna + 1;     
    } else if (direcao == 2) {
        l = linha + 1;      
    } else if (direcao == 3) {
        c = coluna - 1;     
    } else {
        return 0;           
    }

    // No modo globo, quem saiu do mapa volta pelo lado oposto
    if (modo_globo == 1 && linhas >= 3) {
        if (l < 0) {
            l = linhas - 1;
        } else if (l >= linhas) {
            l = 0;
        }
    }
    if (modo_globo == 1 && colunas >= 3) {
        if (c < 0) {
            c = colunas - 1;
        } else if (c >= colunas) {
            c = 0;
        }
    }

    //Fora do mapa ou parede: nao ha vizinho
    if (!labirinto_eh_transitavel(lab, l, c)) {
        return 0;
    }

    *nova_linha = l;
    *nova_coluna = c;
    return 1;
}
