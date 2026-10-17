#include "busca.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    const Labirinto *labirinto;
    unsigned char *visitadas;
    Posicao *caminho_atual;
    int todos_caminhos;
    int parar;
    int erro;
    ResultadoBusca *resultado;
} EstadoBusca;

static size_t indice(const Labirinto *labirinto, size_t linha, size_t coluna) {
    return linha * labirinto->colunas + coluna;
}

static int guardar_caminho(EstadoBusca *estado, size_t tamanho) {
    Posicao *copia;
    Caminho *novos;

    if (tamanho > SIZE_MAX / sizeof(*copia) ||
        estado->resultado->quantidade >= SIZE_MAX / sizeof(*novos)) {
        return -1;
    }

    copia = malloc(tamanho * sizeof(*copia));
    if (copia == NULL) return -1;
    for (size_t i = 0; i < tamanho; ++i) {
        copia[i] = estado->caminho_atual[i];
    }

    novos = realloc(estado->resultado->caminhos,
                    (estado->resultado->quantidade + 1) * sizeof(*novos));
    if (novos == NULL) {
        free(copia);
        return -1;
    }

    estado->resultado->caminhos = novos;
    novos[estado->resultado->quantidade].posicoes = copia;
    novos[estado->resultado->quantidade].tamanho = tamanho;
    estado->resultado->quantidade++;
    return 0;
}

static void explorar(EstadoBusca *estado, size_t linha, size_t coluna,
                    size_t chaves, size_t tamanho, size_t nivel) {
    static const int desloca_linha[4] = {-1, 0, 1, 0};
    static const int desloca_coluna[4] = {0, 1, 0, -1};
    const Labirinto *lab = estado->labirinto;
    char sala;

    if (estado->parar || estado->erro) return;

    estado->resultado->chamadas_recursivas++;
    if (nivel > estado->resultado->nivel_maximo) {
        estado->resultado->nivel_maximo = nivel;
    }

    sala = lab->celulas[linha][coluna];
    if (sala == 'C') chaves++;

    estado->visitadas[indice(lab, linha, coluna)] = 1;
    estado->caminho_atual[tamanho] = (Posicao){linha, coluna};
    tamanho++;

    if (sala == 'X') {
        if (chaves == lab->quantidade_chaves &&
            guardar_caminho(estado, tamanho) != 0) {
            estado->erro = 1;
        }
        if (!estado->todos_caminhos && estado->resultado->quantidade > 0) {
            estado->parar = 1;
        }
    } else {
        for (int direcao = 0; direcao < 4 && !estado->parar && !estado->erro;
             ++direcao) {
            size_t nova_linha = linha;
            size_t nova_coluna = coluna;

            if (desloca_linha[direcao] < 0) {
                if (linha == 0) continue;
                nova_linha--;
            } else if (desloca_linha[direcao] > 0) {
                if (linha + 1 >= lab->linhas) continue;
                nova_linha++;
            }

            if (desloca_coluna[direcao] < 0) {
                if (coluna == 0) continue;
                nova_coluna--;
            } else if (desloca_coluna[direcao] > 0) {
                if (coluna + 1 >= lab->colunas) continue;
                nova_coluna++;
            }

            if (lab->celulas[nova_linha][nova_coluna] == '1' ||
                estado->visitadas[indice(lab, nova_linha, nova_coluna)]) {
                continue;
            }

            explorar(estado, nova_linha, nova_coluna, chaves,
                     tamanho, nivel + 1);
        }
    }

    estado->visitadas[indice(lab, linha, coluna)] = 0;
}

void busca_resultado_liberar(ResultadoBusca *resultado) {
    if (resultado == NULL) return;
    for (size_t i = 0; i < resultado->quantidade; ++i) {
        free(resultado->caminhos[i].posicoes);
    }
    free(resultado->caminhos);
    *resultado = (ResultadoBusca){0};
}

int busca_resolver(const Labirinto *labirinto, int todos_caminhos,
                   ResultadoBusca *resultado) {
    EstadoBusca estado = {0};
    size_t total_celulas;
    unsigned char *visitadas;
    Posicao *caminho_atual;

    if (resultado == NULL) return -1;
    *resultado = (ResultadoBusca){0};

    if (labirinto == NULL || labirinto->celulas == NULL ||
        labirinto->linhas == 0 || labirinto->colunas == 0 ||
        (todos_caminhos != 0 && todos_caminhos != 1) ||
        labirinto->linhas > SIZE_MAX / labirinto->colunas) {
        return -1;
    }

    total_celulas = labirinto->linhas * labirinto->colunas;
    if (total_celulas > SIZE_MAX / sizeof(*caminho_atual) ||
        labirinto->inicio.linha >= labirinto->linhas ||
        labirinto->inicio.coluna >= labirinto->colunas) {
        return -1;
    }

    visitadas = calloc(total_celulas, sizeof(*visitadas));
    caminho_atual = malloc(total_celulas * sizeof(*caminho_atual));
    if (visitadas == NULL || caminho_atual == NULL) {
        free(visitadas);
        free(caminho_atual);
        return -1;
    }

    estado.labirinto = labirinto;
    estado.visitadas = visitadas;
    estado.caminho_atual = caminho_atual;
    estado.todos_caminhos = todos_caminhos;
    estado.resultado = resultado;

    if (labirinto->celulas[labirinto->inicio.linha]
                         [labirinto->inicio.coluna] != '1') {
        explorar(&estado, labirinto->inicio.linha, labirinto->inicio.coluna,
                 0, 0, 1);
    }

    free(visitadas);
    free(caminho_atual);

    if (estado.erro) {
        busca_resultado_liberar(resultado);
        return -1;
    }
    return resultado->quantidade > 0 ? 1 : 0;
}
