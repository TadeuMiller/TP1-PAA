# Contrato de integracao — modulo da Pessoa 1

## Arquivos a incorporar

- `src/labirinto.h` e `src/labirinto.c`.
- **Nao** adicionar `extras/gerador_mapas.c` ao executavel principal: ele tem seu proprio `main`.
- **Nao** adicionar `testes/teste_leitura.c` ou `testes/teste_gerador.c` ao executavel principal: eles tem `main` para testes isolados.

## Tipos e acesso

```c
typedef struct {
    size_t linha;
    size_t coluna;
} Posicao;

typedef struct {
    size_t linhas;
    size_t colunas;
    size_t quantidade_chaves;
    char **celulas;
    Posicao inicio;
    Posicao bau;
    Posicao *chaves;
} Labirinto;
```

Acesso ao mapa: `lab->celulas[linha][coluna]` (indice inicial 0); valores: `A`, `C`, `X`, `0`, `1`. A linha de cada `celulas[i]` tambem e terminada em `\0`, para depuracao/impressao.

`lab->inicio`, `lab->bau`: posicoes da origem e do destino.

`lab->chaves[k]`: posicao da chave k, para `0 <= k < quantidade_chaves`. Quando a quantidade e zero, `chaves` pode ser `NULL`.

## API publica

```c
int labirinto_carregar(const char *caminho, Labirinto **saida,
                       char *erro, size_t capacidade_erro);
void labirinto_liberar(Labirinto *lab);
int labirinto_em_limites(const Labirinto *lab, int linha, int coluna);
int labirinto_eh_transitavel(const Labirinto *lab, int linha, int coluna);
```

- `labirinto_carregar`: 0 em sucesso e -1 em falha. Em sucesso, `*saida` aponta para a estrutura alocada. Em falha, `*saida == NULL`; se fornecido, o buffer `erro` contem uma mensagem.
- `labirinto_liberar`: libera matriz, vetor de chaves e estrutura (aceita NULL).
- `labirinto_em_limites`: verifica os limites da matriz.
- `labirinto_eh_transitavel`: verifica limites e se o conteudo da celula e diferente de `1`. Ela **nao** verifica se a celula foi visitada; isso pertence ao backtracking.

## Exemplo de uso pela Pessoa 3

```c
#include <stdio.h>
#include "labirinto.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s arquivo_mapa.txt\n", argv[0]);
        return 1;
    }
    Labirinto *lab = NULL;
    char erro[256];
    if (labirinto_carregar(argv[1], &lab, erro, sizeof erro) != 0) {
        fprintf(stderr, "Erro: %s\n", erro);
        return 1;
    }

    /* Aqui a Pessoa 3 invoca a busca implementada pela Pessoa 2. */
    /* Por exemplo: busca_resolver(lab, ...); (assinatura a definir com o grupo) */

    printf("Mapa: %zux%zu; inicio [%zu,%zu], bau [%zu,%zu], chaves %zu\n",
           lab->linhas, lab->colunas,
           lab->inicio.linha, lab->inicio.coluna,
           lab->bau.linha, lab->bau.coluna, lab->quantidade_chaves);
    labirinto_liberar(lab);
    return 0;
}
```

Compilar o principal (exemplo; a Pessoa 3 define os demais modulos):

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Isrc src/labirinto.c src/main.c src/busca.c -o programa
```

O exemplo acima e apenas ilustrativo; os nomes `main.c` e `busca.c` dependem do grupo. **Nao e uma afirmacao de que esses arquivos ja existem.**

## Decisoes a alinhar antes de integrar

1. Confirmar que as Pessoas 2 e 3 usarao a `struct Labirinto`, `Posicao` e as assinaturas acima, ou solicitar alteracoes antes de iniciar a integracao.
2. Definir contrato da Pessoa 2 para devolver sucesso, coordenadas do caminho, total de chamadas recursivas e profundidade maxima.
3. Definir se a busca mantem `visitados` separadamente, em vez de sobrescrever `A`, `C` e `X`.
4. Informar a Pessoa 4 de que o gerador e um extra e de que o programa base permanece em C.

## Observacao sobre o enunciado

A lista de coordenadas impressa no exemplo da pagina 3 contem repeticoes, mas o texto da pagina 2 proibe revisitar uma sala dentro do caminho. A implementacao da Pessoa 1 so le entradas. A interpretacao do caminho deve ser discutida com a Pessoa 2/professor se necessario.
