# TP1 de PAA — Pessoa 1: Entrada, estruturas e gerador de mapas

**Projeto:** Projeto e Análise de Algoritmos — UFV, campus Florestal — 2026/2  
**Escopo:** somente a parte da Pessoa 1, com um extra opcional.

## O que vai para o trabalho em grupo

- `src/labirinto.h`: interface publica das estruturas e das funcoes de leitura.
- `src/labirinto.c`: leitura e validacao do arquivo de entrada, memoria e auxiliares.
- `extras/gerador_mapas.c`: **extra opcional** que cria mapas aleatorios e mapas com solucao garantida.
- `docs/relatorio_pessoa1.md` / `docs/relatorio_pessoa1.pdf`: texto pronto para a Pessoa 4 incorporar na documentacao final, sujeito a adequacao as diretrizes do professor.
- `testes/casos/*.txt`: arquivos de entrada, validos e invalidos, para testar a leitura.
- `testes/teste_leitura.c`, `testes/teste_gerador.c`, `testes/rodar_testes.sh`: testes locais independentes da Pessoa 3.
- `docs/contrato_integracao.md`: como as Pessoas 2 e 3 devem consumir a leitura.
- `docs/mensagem_para_grupo.txt`: explicacao informal pronta para enviar no grupo.

**Atenção:** `testes/teste_leitura.c`, `testes/teste_gerador.c` e `extras/gerador_mapas.c` possuem `main()` para testes/extra. O executavel **principal** do projeto deve usar somente `src/labirinto.c` + `src/labirinto.h` como modulo da Pessoa 1. A Pessoa 3 cuida do Makefile definitivo.

## Formato aceito

```text
4 5
2
A01C0
00010
01000
0C00X
```

- Primeira linha: numero de linhas, numero de colunas.
- Segunda linha: quantidade de chaves (`C`).
- A seguir: exatamente uma linha de celulas para cada linha do labirinto.
- `A` = inicio; `C` = chave; `X` = bau; `0` = sala livre; `1` = parede.
- Aceita espacos ou tabs entre celulas (o exemplo do enunciado tem uma linha espacada).
- Exige **exatamente um** `A`, **exatamente um** `X`, e total de `C` igual ao cabecalho.
- Rejeita caracteres invalidos, dimensoes nulas, mapa incompleto ou linhas com tamanho incorreto.
- Um mapa sintaticamente valido *pode nao ter solucao*: decidir isso cabe a Pessoa 2.
- Coordenadas sao indexadas de zero: primeira celula `[0,0]`.

## Como testar no Linux, Git Bash ou WSL

Na raiz deste diretorio:

```bash
bash testes/rodar_testes.sh
```

Ou, manualmente:

```bash
mkdir -p bin
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror -Isrc src/labirinto.c testes/teste_leitura.c -o bin/teste_leitura
./bin/teste_leitura

gcc -std=c11 -Wall -Wextra -Wpedantic -Werror extras/gerador_mapas.c -o bin/gerador_mapas
./bin/gerador_mapas --linhas 8 --colunas 10 --chaves 3 --paredes 30 --saida bin/mapa.txt --semente 42 --garantir-solucao
```

### Windows (PowerShell, com GCC/MinGW instalado)

Execute a partir da raiz do projeto:

```powershell
mkdir bin -ErrorAction SilentlyContinue
gcc -std=c11 -Wall -Wextra -Wpedantic -Isrc src/labirinto.c testes/teste_leitura.c -o bin/teste_leitura.exe
.\bin\teste_leitura.exe

gcc -std=c11 -Wall -Wextra -Wpedantic extras/gerador_mapas.c -o bin/gerador_mapas.exe
.\bin\gerador_mapas.exe --linhas 8 --colunas 10 --chaves 3 --paredes 30 --saida bin/mapa.txt --semente 42 --garantir-solucao
```

## Comandos do extra

```bash
./bin/gerador_mapas --linhas 5 --colunas 7 --chaves 2 --paredes 10 --saida bin/mapa_aleatorio.txt --semente 21
./bin/gerador_mapas --linhas 5 --colunas 7 --chaves 2 --paredes 10 --saida bin/mapa_com_solucao.txt --semente 21 --garantir-solucao
```

Parametros:

| Parametro | Papel |
|---|---|
| `--linhas N` | Altura do labirinto em celulas (N > 0). |
| `--colunas N` | Largura do labirinto em celulas (N > 0). |
| `--chaves N` | Numero exato de celulas `C`. |
| `--paredes N` | Numero **exato** de celulas `1` (nao percentual). |
| `--saida ARQUIVO` | Caminho em que gravar o mapa `.txt`. |
| `--semente N` | Torna a geracao reproduzivel com a mesma implementacao. Opcional. |
| `--garantir-solucao` | Reserva um caminho serpentino simples que inclui todas as chaves ate `X`. Opcional. |

O gerador se recusa a criar mapas sem celulas suficientes para `A`, `X`, chaves e paredes. Sem `--garantir-solucao`, o mapa pode ou nao ter um caminho valido; isso e intencional para testes. Com a opcao, pelo menos um caminho sem repeticao e garantido, mesmo com paredes fora do percurso reservado.

## Contrato com colegas

**Pessoa 2 (backtracking):** ler `docs/contrato_integracao.md`. Recebe `Labirinto *` com `linhas`, `colunas`, `celulas`, `inicio`, `bau`, `chaves` e `quantidade_chaves`. Deve implementar a busca recursiva e a propria matriz de visitados, sem modificar o mapa original se possivel.

**Pessoa 3 (main/Makefile):** deve incluir `src/labirinto.h`, chamar `labirinto_carregar(...)` e liberar via `labirinto_liberar(...)`; compilar `src/labirinto.c` com seus outros fontes. O gerador e um executavel **separado**.

**Pessoa 4 (relatorio/entrega):** incorporar a documentacao, revisar as diretrizes do PVANet e preservar os arquivos de entrada de teste.

## Limites do que foi implementado

Este modulo NAO procura caminhos, NAO mede chamadas recursivas, NAO imprime o caminho final e NAO integra o projeto completo (tarefas das Pessoas 2, 3 e 4). Os testes provam que a leitura valida funciona nos casos ensaiados e que o gerador emite mapas conformes; a integracao completa depende dos colegas.

A saida ilustrativa do enunciado parece conter coordenadas repetidas, enquanto as restricoes pedem nao revisitar uma sala no mesmo caminho. Para a busca (Pessoa 2), convem priorizar as restricoes textuais e consultar o professor se houver duvida.
