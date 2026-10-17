/*
 * Teste do modulo globo. Compilar e rodar na raiz do projeto:
 *   gcc -std=c11 -Wall -Wextra -Wpedantic -Isrc src/labirinto.c src/globo.c \
 *       testes_pessoa4/teste_globo.c -o teste_globo
 *   ./teste_globo
 *
 * A busca daqui e minima: so serve para exercitar globo_vizinho().
 * A busca oficial do projeto e a da Pessoa 2.
 */
#include <stdio.h>
#include <stdlib.h>

#include "globo.h"
#include "labirinto.h"

#define MAX 100

static int falhas = 0;
static int visitado[MAX][MAX];
static int chaves_necessarias;

static void conferir(int condicao, const char *descricao) {
    if (condicao) {
        printf("[OK] %s\n", descricao);
    } else {
        printf("[FALHA] %s\n", descricao);
        falhas++;
    }
}

/* Conta quantos caminhos validos levam ao bau com todas as chaves. */
static long contar(const Labirinto *lab, int linha, int coluna,
                   int chaves, int modo_globo) {
    long total = 0;
    int d, nl, nc;
    char sala = lab->celulas[linha][coluna];

    if (sala == 'C') {
        chaves++;
    }
    if (sala == 'X') {
        if (chaves == chaves_necessarias) {
            return 1;
        }
        return 0;
    }

    visitado[linha][coluna] = 1;
    for (d = 0; d < GLOBO_NUM_DIRECOES; d++) {
        if (globo_vizinho(lab, linha, coluna, d, modo_globo, &nl, &nc)
            && visitado[nl][nc] == 0) {
            total = total + contar(lab, nl, nc, chaves, modo_globo);
        }
    }
    visitado[linha][coluna] = 0;
    return total;
}

/* Le o mapa e devolve o numero de caminhos validos (-1 se nao conseguir ler). */
static long caminhos(const char *arquivo, int modo_globo) {
    Labirinto *lab = NULL;
    char erro[256];
    long total;

    if (labirinto_carregar(arquivo, &lab, erro, sizeof(erro)) != 0) {
        printf("ERRO ao ler %s: %s\n", arquivo, erro);
        return -1;
    }
    chaves_necessarias = (int)lab->quantidade_chaves;
    total = contar(lab, (int)lab->inicio.linha, (int)lab->inicio.coluna,
                   0, modo_globo);
    labirinto_liberar(lab);
    return total;
}

static void testar_vizinhos(void) {
    Labirinto *lab = NULL;
    char erro[256];
    int l = 0, c = 0, achou;

    /* g01: "C A 1 1 1 X" (1 linha, 6 colunas) */
    if (labirinto_carregar("testes_pessoa4/entradas_globo/g01_borda_horizontal.txt",
                           &lab, erro, sizeof(erro)) != 0) {
        printf("ERRO: %s\n", erro);
        falhas++;
        return;
    }

    achou = globo_vizinho(lab, 0, 1, 3, 0, &l, &c);
    conferir(achou == 1 && l == 0 && c == 0, "esquerda de [0,1] -> [0,0]");

    achou = globo_vizinho(lab, 0, 0, 3, 0, &l, &c);
    conferir(achou == 0, "sem globo: esquerda de [0,0] sai do mapa");

    achou = globo_vizinho(lab, 0, 0, 3, 1, &l, &c);
    conferir(achou == 1 && l == 0 && c == 5, "globo: esquerda de [0,0] -> [0,5] (o bau)");

    achou = globo_vizinho(lab, 0, 5, 1, 1, &l, &c);
    conferir(achou == 1 && l == 0 && c == 0, "globo: direita de [0,5] -> [0,0]");

    achou = globo_vizinho(lab, 0, 1, 0, 1, &l, &c);
    conferir(achou == 0, "globo: mapa de 1 linha nao tem vizinho acima");

    achou = globo_vizinho(lab, 0, 1, 2, 1, &l, &c);
    conferir(achou == 0, "globo: mapa de 1 linha nao tem vizinho abaixo");

    achou = globo_vizinho(lab, 0, 1, 4, 1, &l, &c);
    conferir(achou == 0, "direcao invalida e recusada");

    achou = globo_vizinho(NULL, 0, 1, 0, 1, &l, &c);
    conferir(achou == 0, "labirinto NULL e recusado");

    labirinto_liberar(lab);

    /* g03: "C A 1 1 X 1": a volta cai numa parede */
    if (labirinto_carregar("testes_pessoa4/entradas_globo/g03_parede_na_borda.txt",
                           &lab, erro, sizeof(erro)) != 0) {
        printf("ERRO: %s\n", erro);
        falhas++;
        return;
    }
    achou = globo_vizinho(lab, 0, 0, 3, 1, &l, &c);
    conferir(achou == 0, "globo: a volta de [0,0] cai na parede [0,5] e e recusada");
    labirinto_liberar(lab);
}

static void testar_busca(void) {
    const char *g1 = "testes_pessoa4/entradas_globo/g01_borda_horizontal.txt";
    const char *g2 = "testes_pessoa4/entradas_globo/g02_borda_vertical.txt";
    const char *g3 = "testes_pessoa4/entradas_globo/g03_parede_na_borda.txt";

    conferir(caminhos(g1, 0) == 0, "g01 sem globo: sem solucao");
    conferir(caminhos(g1, 1) == 1, "g01 com globo: 1 caminho");
    conferir(caminhos(g2, 0) == 0, "g02 sem globo: sem solucao");
    conferir(caminhos(g2, 1) == 1, "g02 com globo: 1 caminho");
    conferir(caminhos(g3, 0) == 0, "g03 sem globo: sem solucao");
    conferir(caminhos(g3, 1) == 0, "g03 com globo: sem solucao (parede)");
}

int main(void) {
    testar_vizinhos();
    testar_busca();

    if (falhas == 0) {
        printf("\nTODOS OS TESTES PASSARAM\n");
        return 0;
    }
    printf("\nHA %d FALHA(S)\n", falhas);
    return 1;
}
