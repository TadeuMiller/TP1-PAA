/* Testes unitarios independentes do main da pessoa 3. */
#include "../src/labirinto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int executados = 0, aprovados = 0;

static void verificar(const char *arquivo, int deve_aceitar,
                      size_t linhas, size_t colunas, size_t chaves,
                      size_t inicio_l, size_t inicio_c,
                      size_t bau_l, size_t bau_c) {
    Labirinto *lab = NULL;
    char erro[256];
    int resultado = labirinto_carregar(arquivo, &lab, erro, sizeof erro);
    int ok = (resultado == 0) == deve_aceitar;
    if (deve_aceitar && ok) {
        ok = lab != NULL && lab->linhas == linhas && lab->colunas == colunas &&
             lab->quantidade_chaves == chaves &&
             lab->inicio.linha == inicio_l && lab->inicio.coluna == inicio_c &&
             lab->bau.linha == bau_l && lab->bau.coluna == bau_c &&
             labirinto_eh_transitavel(lab, (int)inicio_l, (int)inicio_c) &&
             !labirinto_eh_transitavel(lab, -1, 0) &&
             !labirinto_eh_transitavel(lab, (int)linhas, 0);
    }
    ++executados;
    if (ok) ++aprovados;
    printf("[%s] %-46s %s\n", ok ? "OK" : "FALHOU", arquivo,
           resultado == 0 ? "leitura aceita" : erro);
    labirinto_liberar(lab);
}

int main(int argc, char **argv) {
    const char *dir = (argc > 1) ? argv[1] : "testes/casos";
    const char *validos[][2] = {
        {"exemplo_enunciado.txt", "10,10,2,0,0,9,9"},
        {"valido_espacos.txt", "4,5,2,0,0,3,4"},
        {"sem_chaves.txt", "2,2,0,0,0,1,1"},
        {"sem_solucao.txt", "3,3,1,0,0,2,2"}
    };
    for (size_t i = 0; i < sizeof validos / sizeof validos[0]; ++i) {
        char caminho[1024];
        size_t dados[7];
        snprintf(caminho, sizeof caminho, "%s/%s", dir, validos[i][0]);
        if (sscanf(validos[i][1], "%zu,%zu,%zu,%zu,%zu,%zu,%zu",
                   &dados[0], &dados[1], &dados[2], &dados[3],
                   &dados[4], &dados[5], &dados[6]) != 7) return 2;
        verificar(caminho, 1, dados[0], dados[1], dados[2],
                  dados[3], dados[4], dados[5], dados[6]);
    }
    const char *invalidos[] = {
        "simbolo_invalido.txt", "falta_bau.txt", "falta_chave.txt",
        "colunas_erradas.txt", "duas_entradas.txt", "linha_extra.txt",
        "dimensao_invalida.txt", "mapa_incompleto.txt", "nao_existe.txt"
    };
    for (size_t i = 0; i < sizeof invalidos / sizeof invalidos[0]; ++i) {
        char caminho[1024];
        snprintf(caminho, sizeof caminho, "%s/%s", dir, invalidos[i]);
        verificar(caminho, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    printf("\nResultado: %d/%d testes aprovados.\n", aprovados, executados);
    return aprovados == executados ? EXIT_SUCCESS : EXIT_FAILURE;
}
