/* Gerador de arquivos de entrada para o TP1 de PAA (extra opcional).
 * Programa independente, em C11, sem bibliotecas externas.
 */
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

static void ajuda(const char *programa) {
    printf("Uso: %s --linhas N --colunas N --chaves N --paredes N --saida ARQUIVO [--semente N] [--garantir-solucao]\n", programa);
    printf("  --paredes N            quantidade EXATA de paredes (nao percentual)\n");
    printf("  --garantir-solucao     reserva um percurso simples de A a X passando por todas as chaves\n");
    printf("  --semente N            semente da aleatoriedade para repetir o mapa\n");
    printf("  --ajuda                mostra estas instrucoes\n");
}

static int numero(const char *texto, size_t *resultado) {
    if (!texto || *texto == '\0' || *texto == '-') return -1;
    errno = 0;
    char *fim = NULL;
    unsigned long long n = strtoull(texto, &fim, 10);
    if (errno == ERANGE || fim == texto || *fim || n > SIZE_MAX) return -1;
    *resultado = (size_t)n;
    return 0;
}

/* Sorteio em [0, limite), adequado para mapas didaticos. */
static size_t sorteio(size_t limite) {
    if (limite <= 1) return 0;
    /* Evita limites acima de RAND_MAX para o embaralhamento. */
    if (limite <= (size_t)RAND_MAX)
        return (size_t)rand() % limite;
    size_t alto = (size_t)(unsigned)rand();
    size_t baixo = (size_t)(unsigned)rand();
    return ((alto << (sizeof(size_t) * CHAR_BIT / 2)) ^ baixo) % limite;
}

static void embaralhar(size_t *v, size_t n) {
    for (size_t i = n; i > 1; --i) {
        size_t j = sorteio(i);
        size_t tmp = v[i - 1];
        v[i - 1] = v[j];
        v[j] = tmp;
    }
}

/* Ordem serpentina: indices consecutivos representam celulas vizinhas. */
static size_t serpentina(size_t passo, size_t colunas) {
    size_t linha = passo / colunas;
    size_t pos_na_linha = passo % colunas;
    size_t coluna = (linha % 2 == 0) ? pos_na_linha : colunas - 1 - pos_na_linha;
    return linha * colunas + coluna;
}

int main(int argc, char **argv) {
    size_t linhas = 0, colunas = 0, chaves = 0, paredes = 0, semente = 0;
    int tem_linhas = 0, tem_colunas = 0, tem_chaves = 0, tem_paredes = 0;
    int tem_semente = 0, garantir = 0;
    const char *saida = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--garantir-solucao") == 0) {
            garantir = 1;
        } else if (strcmp(argv[i], "--ajuda") == 0) {
            ajuda(argv[0]);
            return 0;
        } else {
            if (i + 1 >= argc) {
                fprintf(stderr, "Falta valor para %s.\n", argv[i]);
                return 1;
            }
            const char *opcao = argv[i];
            const char *valor = argv[++i];
            if (strcmp(opcao, "--linhas") == 0) {
                tem_linhas = numero(valor, &linhas) == 0;
                if (!tem_linhas) goto entrada_invalida;
            } else if (strcmp(opcao, "--colunas") == 0) {
                tem_colunas = numero(valor, &colunas) == 0;
                if (!tem_colunas) goto entrada_invalida;
            } else if (strcmp(opcao, "--chaves") == 0) {
                tem_chaves = numero(valor, &chaves) == 0;
                if (!tem_chaves) goto entrada_invalida;
            } else if (strcmp(opcao, "--paredes") == 0) {
                tem_paredes = numero(valor, &paredes) == 0;
                if (!tem_paredes) goto entrada_invalida;
            } else if (strcmp(opcao, "--semente") == 0) {
                tem_semente = numero(valor, &semente) == 0;
                if (!tem_semente) goto entrada_invalida;
            } else if (strcmp(opcao, "--saida") == 0) {
                saida = valor;
            } else {
                fprintf(stderr, "Opcao desconhecida: %s\n", opcao);
                return 1;
            }
        }
    }
    if (!tem_linhas || !tem_colunas || !tem_chaves || !tem_paredes ||
        !saida || linhas == 0 || colunas == 0 || linhas > SIZE_MAX / colunas) {
        fprintf(stderr, "Forneca dimensoes positivas, chaves, paredes e arquivo de saida.\n");
        ajuda(argv[0]);
        return 1;
    }
    size_t total = linhas * colunas;
    if (total < 2 || chaves > total - 2 || paredes > total - chaves - 2 ||
        total > SIZE_MAX / sizeof(size_t)) {
        fprintf(stderr, "Impossivel acomodar A, X, %zu chaves e %zu paredes.\n",
                chaves, paredes);
        return 1;
    }
    if (!tem_semente) semente = (size_t)time(NULL);
    srand((unsigned)semente);

    char *mapa = malloc(total);
    size_t *indices = malloc(total * sizeof(*indices));
    if (!mapa || !indices) {
        fprintf(stderr, "Memoria insuficiente para gerar o mapa.\n");
        free(mapa);
        free(indices);
        return 1;
    }
    memset(mapa, '0', total);

    if (garantir) {
        size_t disponiveis = total - paredes;
        /* Um pouco de corredor entre as chaves quando ha espaco disponivel. */
        size_t caminho = chaves + 2;
        size_t extras = (disponiveis - caminho < 6) ? disponiveis - caminho : 6;
        caminho += extras;
        /* Escolhe um prefixo de um percurso serpentino sem repeticoes. */
        for (size_t i = 0; i < caminho; ++i) {
            size_t idx = serpentina(i, colunas);
            mapa[idx] = '.';  /* celula reservada ate a atribuicao final */
        }
        size_t livres = 0;
        for (size_t i = 0; i < total; ++i)
            if (mapa[i] == '0') indices[livres++] = i;
        embaralhar(indices, livres);
        for (size_t i = 0; i < paredes; ++i) mapa[indices[i]] = '1';
        /* Sorteia as chaves entre as celulas internas do corredor. */
        for (size_t i = 1; i + 1 < caminho; ++i)
            indices[i - 1] = serpentina(i, colunas);
        embaralhar(indices, caminho - 2);
        for (size_t i = 0; i < chaves; ++i) mapa[indices[i]] = 'C';
        for (size_t i = 0; i < total; ++i)
            if (mapa[i] == '.') mapa[i] = '0';
        mapa[serpentina(0, colunas)] = 'A';
        mapa[serpentina(caminho - 1, colunas)] = 'X';
    } else {
        for (size_t i = 0; i < total; ++i) indices[i] = i;
        embaralhar(indices, total);
        for (size_t i = 0; i < paredes; ++i) mapa[indices[i]] = '1';
        mapa[indices[paredes]] = 'A';
        mapa[indices[paredes + 1]] = 'X';
        for (size_t i = 0; i < chaves; ++i)
            mapa[indices[paredes + 2 + i]] = 'C';
    }

    FILE *arquivo = fopen(saida, "w");
    if (!arquivo) {
        fprintf(stderr, "Nao foi possivel criar: %s\n", saida);
        free(mapa);
        free(indices);
        return 1;
    }
    int falha = fprintf(arquivo, "%zu %zu\n%zu\n", linhas, colunas, chaves) < 0;
    for (size_t i = 0; i < linhas && !falha; ++i) {
        if (fwrite(mapa + i * colunas, 1, colunas, arquivo) != colunas ||
            fputc('\n', arquivo) == EOF) falha = 1;
    }
    if (fclose(arquivo) != 0) falha = 1;
    free(mapa);
    free(indices);
    if (falha) {
        fprintf(stderr, "Erro na escrita do mapa.\n");
        return 1;
    }
    printf("Mapa gerado: %s (%zux%zu, %zu chaves, %zu paredes, semente %zu, %s).\n",
           saida, linhas, colunas, chaves, paredes, semente,
           garantir ? "com solucao garantida" : "aleatorio sem garantia de solucao");
    return 0;

entrada_invalida:
    fprintf(stderr, "Valor numerico invalido.\n");
    return 1;
}
