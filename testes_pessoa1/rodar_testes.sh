#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
CC="${CC:-gcc}"
mkdir -p bin
"$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 -Isrc src/labirinto.c testes/teste_leitura.c -o bin/teste_leitura
"$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 extras/gerador_mapas.c -o bin/gerador_mapas
"$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror -O2 src/labirinto.c testes/teste_gerador.c -o bin/teste_gerador
./bin/teste_leitura
./bin/gerador_mapas --linhas 8 --colunas 10 --chaves 3 --paredes 30 --saida bin/gerado_com_solucao.txt --semente 42 --garantir-solucao
./bin/gerador_mapas --linhas 5 --colunas 5 --chaves 2 --paredes 5 --saida bin/gerado_aleatorio.txt --semente 42
./bin/teste_gerador bin/gerado_com_solucao.txt 8 10 3 30
./bin/teste_gerador bin/gerado_aleatorio.txt 5 5 2 5
./bin/gerador_mapas --linhas 1 --colunas 6 --chaves 3 --paredes 1 --saida bin/com_solucao_uma_linha.txt --semente 5 --garantir-solucao
./bin/teste_gerador bin/com_solucao_uma_linha.txt 1 6 3 1
printf '\nCompilacao, testes e geracao de exemplos concluidos.\n'
