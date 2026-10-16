/* Valida formato, contagem de paredes/chaves e o corredor garantido pelo extra. */
#include "../src/labirinto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t serpentina(size_t n, size_t colunas) {
    size_t linha = n / colunas;
    size_t coluna_na_linha = n % colunas;
    return linha * colunas + ((linha % 2) ? colunas - 1 - coluna_na_linha : coluna_na_linha);
}

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "Uso: teste_gerador ARQUIVO LINHAS COLUNAS CHAVES PAREDES \n");
        return 2;
    }
    Labirinto *l = NULL;
    char erro[256];
    if (labirinto_carregar(argv[1], &l, erro, sizeof erro) != 0) {
        fprintf(stderr, "Arquivo gerado nao passou no leitor: %s\n", erro);
        return 1;
    }
    size_t exp_l = (size_t)strtoull(argv[2], NULL, 10);
    size_t exp_c = (size_t)strtoull(argv[3], NULL, 10);
    size_t exp_k = (size_t)strtoull(argv[4], NULL, 10);
    size_t exp_w = (size_t)strtoull(argv[5], NULL, 10);
    size_t paredes = 0, keys = 0, inicio = 0, bau = 0;
    for (size_t i = 0; i < l->linhas; ++i)
        for (size_t j = 0; j < l->colunas; ++j) {
            char c = l->celulas[i][j];
            paredes += c == '1';
            keys += c == 'C';
            inicio += c == 'A';
            bau += c == 'X';
        }
    int ok = l->linhas == exp_l && l->colunas == exp_c &&
             l->quantidade_chaves == exp_k && paredes == exp_w &&
             keys == exp_k && inicio == 1 && bau == 1;
    if (ok) printf("[OK] Gerado %s: %zux%zu, %zu chaves, %zu paredes.\n",
                   argv[1], exp_l, exp_c, exp_k, exp_w);
    else fprintf(stderr, "[ERRO] Arquivo gerado com contagens incorretas.\n");

    /* So para um mapa criado com --garantir-solucao: se o bau esta no
     * prefixo da serpentina, todo o caminho ate ele precisa ser livre. */
    if (ok && strstr(argv[1], "com_solucao") != NULL) {
        size_t chaves_no_caminho = 0, chegada = 0;
        for (size_t passo = 0; passo < l->linhas * l->colunas; ++passo) {
            size_t idx = serpentina(passo, l->colunas);
            size_t linha = idx / l->colunas, coluna = idx % l->colunas;
            char c = l->celulas[linha][coluna];
            if (c == '1') { ok = 0; break; }
            if (passo == 0 && c != 'A') { ok = 0; break; }
            if (c == 'C') ++chaves_no_caminho;
            if (c == 'X') { chegada = 1; break; }
        }
        if (!chegada || chaves_no_caminho != l->quantidade_chaves) ok = 0;
        printf("[%s] Corredor sem repeticao coletou %zu/%zu chaves ate o bau.\n",
               ok ? "OK" : "ERRO", chaves_no_caminho, l->quantidade_chaves);
    }
    labirinto_liberar(l);
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
