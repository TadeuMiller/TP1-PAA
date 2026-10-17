# Trabalho Prático 1 — Parte da Pessoa 2

## Busca por backtracking

O módulo `src/busca.c` recebe um `Labirinto` já carregado pelo módulo da Pessoa 1. A busca começa em `labirinto->inicio`, percorre apenas as quatro direções ortogonais e não altera as células do mapa. Uma matriz auxiliar registra as salas visitadas no caminho atual, impedindo paredes e repetições.

Ao entrar em uma sala `C`, a busca incrementa a quantidade de chaves coletadas. Uma solução é registrada quando a busca chega a `X` com a quantidade de chaves exigida pelo mapa. Ao retornar de uma tentativa, a sala é desmarcada para que possa ser explorada em outro caminho. Salas fora dos limites e paredes não são exploradas.

## Interface e resultados

`busca_resolver` recebe o labirinto, uma opção `todos_caminhos` e um `ResultadoBusca`. Com a opção desligada, a busca termina na primeira solução. Com a opção ligada, continua e guarda todos os caminhos válidos. A função retorna `1` quando encontra ao menos uma solução, `0` quando não encontra e `-1` em caso de argumento inválido ou falha de alocação.

Cada caminho contém uma sequência de `Posicao` indexadas a partir de zero. O resultado também contém a quantidade de caminhos, o total de chamadas recursivas e o nível máximo. A chamada inicial é contada como nível 1. O chamador deve liberar os caminhos com `busca_resultado_liberar` antes de reutilizar o resultado.

## Contadores e complexidade

Uma chamada é contabilizada cada vez que a função recursiva entra em uma sala. O nível aumenta em uma unidade a cada movimento. Como cada sala aparece no máximo uma vez em um caminho, a profundidade máxima é limitada pelo número de salas `V`.

No pior caso, a busca pode explorar um número exponencial de caminhos simples. Como cada sala tem no máximo quatro vizinhas e, depois do primeiro movimento, a sala anterior já está visitada, um limite superior para a árvore de busca é `O(3^V)`. A busca usa `O(V)` de memória auxiliar para o caminho atual, as salas visitadas e a pilha recursiva. No modo de todos os caminhos, também armazena as soluções encontradas, usando `O(Σ L)` de memória adicional, onde `L` é o tamanho de cada caminho salvo.

## Extra: todos os caminhos

O extra está disponível pelo parâmetro `todos_caminhos = 1`. Nesse modo, a busca não encerra ao encontrar a primeira solução; copia e guarda cada caminho que chega ao baú com todas as chaves. Com `todos_caminhos = 0`, permanece o comportamento básico exigido.
