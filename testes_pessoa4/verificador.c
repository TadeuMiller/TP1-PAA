#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LIMITE 100

typedef struct {
    int linhas;
    int colunas;
    int chaves;
    char salas[LIMITE][LIMITE];
} Mapa;

typedef struct {
    int linha;
    int coluna;
} Posicao;

int lerMapa(const char *nome, Mapa *mapa) {
    FILE *arquivo = fopen(nome, "r");
    int i, j, inicios = 0, baus = 0, chaves = 0;
    char sala, extra;

    if (arquivo == NULL) {
        printf("ERRO: nao foi possivel abrir o mapa.\n");
        return 0;
    }
    if (fscanf(arquivo, "%d %d %d", &mapa->linhas,
               &mapa->colunas, &mapa->chaves) != 3) {
        printf("ERRO: cabecalho incompleto ou com valores invalidos.\n");
        fclose(arquivo);
        return 0;
    }
    if (mapa->linhas < 1 || mapa->linhas > LIMITE ||
        mapa->colunas < 1 || mapa->colunas > LIMITE) {
        printf("ERRO: dimensoes devem estar entre 1 e 100.\n");
        fclose(arquivo);
        return 0;
    }
    if (mapa->chaves < 0 ||
        mapa->chaves > mapa->linhas * mapa->colunas - 2) {
        printf("ERRO: quantidade de chaves invalida para as dimensoes do mapa.\n");
        fclose(arquivo);
        return 0;
    }

    /* O espaco antes de %c permite celulas separadas por espacos. */
    for (i = 0; i < mapa->linhas; i++) {
        for (j = 0; j < mapa->colunas; j++) {
            if (fscanf(arquivo, " %c", &sala) != 1 ||
                (sala != 'A' && sala != 'X' && sala != 'C' &&
                 sala != '0' && sala != '1')) {
                printf("ERRO: celula ausente ou invalida no mapa.\n");
                fclose(arquivo);
                return 0;
            }
            mapa->salas[i][j] = sala;
            if (sala == 'A') inicios++;
            if (sala == 'X') baus++;
            if (sala == 'C') chaves++;
        }
    }
    if (fscanf(arquivo, " %c", &extra) == 1 ||
        inicios != 1 || baus != 1 || chaves != mapa->chaves) {
        printf("ERRO: o mapa deve ter um A, um X e exatamente as chaves informadas.\n");
        fclose(arquivo);
        return 0;
    }
    fclose(arquivo);
    return 1;
}

int temMensagemSemSolucao(FILE *arquivo) {
    const char *mensagem =
        "Não há um caminho possível para a entrada de dados fornecida";
    char linha[1024];
    int inicio, fim;

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        inicio = 0;
        while (isspace((unsigned char)linha[inicio])) inicio++;
        fim = (int)strlen(linha);
        while (fim > inicio && isspace((unsigned char)linha[fim - 1])) fim--;
        linha[fim] = '\0';
        if (strcmp(linha + inicio, mensagem) == 0) {
            rewind(arquivo);
            return 1;
        }
    }
    rewind(arquivo);
    return 0;
}

int lerCoordenada(FILE *arquivo, Posicao *posicao) {
    int caractere;
    char fechamento;

    /* Texto e metricas fora dos colchetes sao ignorados. */
    do {
        caractere = fgetc(arquivo);
    } while (caractere != EOF && caractere != '[');
    if (caractere == EOF) return 0;

    if (fscanf(arquivo, " %d , %d %c", &posicao->linha,
               &posicao->coluna, &fechamento) != 3 || fechamento != ']') {
        printf("FALHA: coordenada malformada. Use [linha,coluna].\n");
        return -1;
    }
    return 1;
}

int conferirPasso(const Mapa *mapa, Posicao atual, Posicao anterior,
                  int passo, int visitadas[LIMITE][LIMITE]) {
    int distancia;

    /* Conferir os limites antes de acessar a matriz. */
    if (atual.linha < 0 || atual.linha >= mapa->linhas ||
        atual.coluna < 0 || atual.coluna >= mapa->colunas) {
        printf("FALHA: coordenada fora do mapa no passo %d.\n", passo);
        return 0;
    }
    if (mapa->salas[atual.linha][atual.coluna] == '1') {
        printf("FALHA: o caminho atravessa uma parede no passo %d.\n", passo);
        return 0;
    }
    if (visitadas[atual.linha][atual.coluna]) {
        printf("FALHA: sala repetida no passo %d.\n", passo);
        return 0;
    }
    if (passo == 1 && mapa->salas[atual.linha][atual.coluna] != 'A') {
        printf("FALHA: o caminho nao comeca em A.\n");
        return 0;
    }
    if (passo > 1) {
        distancia = abs(atual.linha - anterior.linha) +
                    abs(atual.coluna - anterior.coluna);
        if (distancia != 1) {
            printf("FALHA: movimento invalido no passo %d.\n", passo);
            return 0;
        }
    }
    visitadas[atual.linha][atual.coluna] = 1;
    return 1;
}

int conferirSemSolucao(FILE *arquivo) {
    Posicao posicao;
    int leitura;

    if (!temMensagemSemSolucao(arquivo)) {
        printf("FALHA: a mensagem exigida de ausencia de solucao nao apareceu.\n");
        return 0;
    }
    leitura = lerCoordenada(arquivo, &posicao);
    if (leitura == -1) {
        return 0;
    }
    if (leitura == 1) {
        printf("FALHA: ha coordenadas em uma resposta sem solucao.\n");
        return 0;
    }
    printf("APROVADO: mensagem de ausencia de solucao conferida.\n");
    return 1;
}

int conferirCaminho(const Mapa *mapa, FILE *arquivo) {
    int visitadas[LIMITE][LIMITE] = {{0}};
    Posicao atual, anterior = {0, 0};
    int leitura, passos = 0, chavesColetadas = 0;

    if (temMensagemSemSolucao(arquivo)) {
        printf("FALHA: era esperado um caminho, mas foi informada ausencia de solucao.\n");
        return 0;
    }

    leitura = lerCoordenada(arquivo, &atual);
    while (leitura == 1) {
        passos++;
        if (!conferirPasso(mapa, atual, anterior, passos, visitadas)) {
            return 0;
        }
        if (mapa->salas[atual.linha][atual.coluna] == 'C') {
            chavesColetadas++;
        }
        anterior = atual;
        leitura = lerCoordenada(arquivo, &atual);
    }
    if (leitura == -1) {
        return 0;
    }
    if (passos == 0) {
        printf("FALHA: era esperado um caminho, mas ele nao foi apresentado.\n");
        return 0;
    }
    if (mapa->salas[anterior.linha][anterior.coluna] != 'X') {
        printf("FALHA: o caminho nao termina em X.\n");
        return 0;
    }
    if (chavesColetadas != mapa->chaves) {
        printf("FALHA: foram coletadas %d de %d chaves.\n",
               chavesColetadas, mapa->chaves);
        return 0;
    }
    printf("APROVADO: caminho valido com %d salas e %d chaves.\n",
           passos, chavesColetadas);
    return 1;
}

int main(int argc, char *argv[]) {
    Mapa mapa;
    FILE *saida;
    int esperaSolucao, aprovado;

    if (argc != 4 || (strcmp(argv[3], "com_solucao") != 0 &&
                      strcmp(argv[3], "sem_solucao") != 0)) {
        printf("Uso: %s mapa.txt saida.txt com_solucao|sem_solucao\n", argv[0]);
        return 2;
    }
    esperaSolucao = strcmp(argv[3], "com_solucao") == 0;
    if (!lerMapa(argv[1], &mapa)) {
        return 2;
    }
    saida = fopen(argv[2], "r");
    if (saida == NULL) {
        printf("ERRO: nao foi possivel abrir a saida.\n");
        return 2;
    }
    if (esperaSolucao) {
        aprovado = conferirCaminho(&mapa, saida);
    } else {
        aprovado = conferirSemSolucao(saida);
    }
    fclose(saida);
    return aprovado ? 0 : 1;
}
