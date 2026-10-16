#include "labirinto.h"

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static void informar_erro(char *buffer, size_t capacidade, const char *formato, ...) {
    if (buffer == NULL || capacidade == 0) return;
    va_list argumentos;
    va_start(argumentos, formato);
    vsnprintf(buffer, capacidade, formato, argumentos);
    va_end(argumentos);
}

/* Le uma linha de qualquer tamanho. 1 = linha lida, 0 = EOF, -1 = erro. */
static int ler_linha(FILE *arquivo, char **linha, size_t *capacidade) {
    size_t tamanho = 0;
    int ch;
    if (*linha == NULL) {
        *capacidade = 128;
        *linha = malloc(*capacidade);
        if (*linha == NULL) return -1;
    }
    while ((ch = fgetc(arquivo)) != EOF && ch != '\n') {
        if (tamanho + 1 >= *capacidade) {
            if (*capacidade > SIZE_MAX / 2) return -1;
            size_t nova_capacidade = *capacidade * 2;
            char *novo = realloc(*linha, nova_capacidade);
            if (novo == NULL) return -1;
            *linha = novo;
            *capacidade = nova_capacidade;
        }
        (*linha)[tamanho++] = (char)ch;
    }
    if (ch == EOF && ferror(arquivo)) return -1;
    if (ch == EOF && tamanho == 0) return 0;
    (*linha)[tamanho] = '\0';
    return 1;
}

static int proximo_numero(const char **texto, size_t *numero) {
    const char *p = *texto;
    while (isspace((unsigned char)*p)) p++;
    if (!isdigit((unsigned char)*p)) return -1;
    errno = 0;
    char *fim = NULL;
    unsigned long long valor = strtoull(p, &fim, 10);
    if (errno == ERANGE || fim == p || valor > SIZE_MAX) return -1;
    *numero = (size_t)valor;
    *texto = fim;
    return 0;
}

static int cabecalho_numerico(const char *linha, size_t *a, size_t *b, int dois) {
    const char *p = linha;
    if (proximo_numero(&p, a) != 0) return -1;
    if (dois && proximo_numero(&p, b) != 0) return -1;
    while (isspace((unsigned char)*p)) p++;
    return *p == '\0' ? 0 : -1;
}

void labirinto_liberar(Labirinto *labirinto) {
    if (labirinto == NULL) return;
    if (labirinto->celulas != NULL) {
        for (size_t linha = 0; linha < labirinto->linhas; ++linha)
            free(labirinto->celulas[linha]);
    }
    free(labirinto->celulas);
    free(labirinto->chaves);
    free(labirinto);
}

int labirinto_em_limites(const Labirinto *labirinto, int linha, int coluna) {
    return labirinto != NULL && linha >= 0 && coluna >= 0 &&
           (size_t)linha < labirinto->linhas &&
           (size_t)coluna < labirinto->colunas;
}

int labirinto_eh_transitavel(const Labirinto *labirinto, int linha, int coluna) {
    return labirinto_em_limites(labirinto, linha, coluna) &&
           labirinto->celulas[linha][coluna] != '1';
}

int labirinto_carregar(const char *caminho, Labirinto **saida,
                       char *erro, size_t capacidade_erro) {
    FILE *arquivo = NULL;
    Labirinto *lab = NULL;
    char *linha_texto = NULL;
    size_t capacidade_linha = 0;
    size_t linhas = 0, colunas = 0, qtd_chaves = 0;
    size_t achadas = 0, inicios = 0, baus = 0;
    int status = -1;
    int lido;

    if (saida != NULL) *saida = NULL;
    if (erro != NULL && capacidade_erro > 0) erro[0] = '\0';
    if (saida == NULL || caminho == NULL) {
        informar_erro(erro, capacidade_erro, "Argumento invalido.");
        return -1;
    }
    arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        informar_erro(erro, capacidade_erro, "Nao foi possivel abrir: %s", caminho);
        return -1;
    }

    lido = ler_linha(arquivo, &linha_texto, &capacidade_linha);
    if (lido != 1 || cabecalho_numerico(linha_texto, &linhas, &colunas, 1) != 0 ||
        linhas == 0 || colunas == 0) {
        informar_erro(erro, capacidade_erro,
                      "Primeira linha invalida: esperadas duas dimensoes positivas.");
        goto limpeza;
    }
    lido = ler_linha(arquivo, &linha_texto, &capacidade_linha);
    if (lido != 1 || cabecalho_numerico(linha_texto, &qtd_chaves, NULL, 0) != 0) {
        informar_erro(erro, capacidade_erro,
                      "Segunda linha invalida: esperada quantidade de chaves >= 0.");
        goto limpeza;
    }
    if (colunas == SIZE_MAX || linhas > SIZE_MAX / sizeof(char *) ||
        linhas > SIZE_MAX / colunas ||
        qtd_chaves > (linhas * colunas >= 2 ? linhas * colunas - 2 : 0) ||
        qtd_chaves > SIZE_MAX / sizeof(Posicao)) {
        informar_erro(erro, capacidade_erro,
                      "Dimensoes ou quantidade de chaves incompativeis.");
        goto limpeza;
    }

    lab = calloc(1, sizeof(*lab));
    if (lab == NULL) {
        informar_erro(erro, capacidade_erro, "Sem memoria para o labirinto.");
        goto limpeza;
    }
    lab->linhas = linhas;
    lab->colunas = colunas;
    lab->quantidade_chaves = qtd_chaves;
    lab->celulas = calloc(linhas, sizeof(*lab->celulas));
    if (qtd_chaves > 0) lab->chaves = malloc(qtd_chaves * sizeof(*lab->chaves));
    if (lab->celulas == NULL || (qtd_chaves > 0 && lab->chaves == NULL)) {
        informar_erro(erro, capacidade_erro, "Sem memoria para armazenar o mapa.");
        goto limpeza;
    }

    for (size_t i = 0; i < linhas; ++i) {
        lido = ler_linha(arquivo, &linha_texto, &capacidade_linha);
        if (lido != 1) {
            informar_erro(erro, capacidade_erro,
                          "Mapa incompleto: faltou a linha %zu.", i);
            goto limpeza;
        }
        lab->celulas[i] = malloc(colunas + 1);
        if (lab->celulas[i] == NULL) {
            informar_erro(erro, capacidade_erro, "Sem memoria para a linha %zu.", i);
            goto limpeza;
        }
        size_t j = 0;
        for (const unsigned char *p = (const unsigned char *)linha_texto; *p; ++p) {
            if (isspace(*p)) continue;
            if (j == colunas) {
                informar_erro(erro, capacidade_erro,
                              "Linha %zu tem mais de %zu colunas.", i, colunas);
                goto limpeza;
            }
            char simbolo = (char)*p;
            if (simbolo != 'A' && simbolo != 'C' && simbolo != 'X' &&
                simbolo != '0' && simbolo != '1') {
                informar_erro(erro, capacidade_erro,
                              "Simbolo invalido '%c' na posicao [%zu,%zu].", simbolo, i, j);
                goto limpeza;
            }
            lab->celulas[i][j] = simbolo;
            if (simbolo == 'A') {
                ++inicios;
                lab->inicio = (Posicao){i, j};
            } else if (simbolo == 'X') {
                ++baus;
                lab->bau = (Posicao){i, j};
            } else if (simbolo == 'C') {
                if (achadas >= qtd_chaves) {
                    informar_erro(erro, capacidade_erro,
                                  "O mapa possui mais chaves C que o cabecalho informa.");
                    goto limpeza;
                }
                lab->chaves[achadas++] = (Posicao){i, j};
            }
            ++j;
        }
        if (j != colunas) {
            informar_erro(erro, capacidade_erro,
                          "Linha %zu possui %zu colunas; esperado %zu.", i, j, colunas);
            goto limpeza;
        }
        lab->celulas[i][colunas] = '\0';
    }
    while ((lido = ler_linha(arquivo, &linha_texto, &capacidade_linha)) == 1) {
        const unsigned char *p = (const unsigned char *)linha_texto;
        while (*p && isspace(*p)) ++p;
        if (*p) {
            informar_erro(erro, capacidade_erro,
                          "Conteudo extra apos as %zu linhas do mapa.", linhas);
            goto limpeza;
        }
    }
    if (lido == -1) {
        informar_erro(erro, capacidade_erro, "Erro ao ler final do arquivo.");
        goto limpeza;
    }
    if (inicios != 1 || baus != 1 || achadas != qtd_chaves) {
        informar_erro(erro, capacidade_erro,
                      "Mapa invalido: A=%zu (esperado 1), X=%zu (esperado 1), C=%zu (esperado %zu).",
                      inicios, baus, achadas, qtd_chaves);
        goto limpeza;
    }
    *saida = lab;
    lab = NULL;
    status = 0;

limpeza:
    labirinto_liberar(lab);
    free(linha_texto);
    fclose(arquivo);
    return status;
}
