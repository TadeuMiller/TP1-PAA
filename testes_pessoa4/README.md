# Testes da pessoa 4 (versao basica)

Conferem se o caminho devolvido pelo programa do labirinto e valido.
Nao fazem parte do programa principal.

## Pastas e arquivos
- `entradas/`: 8 mapas de teste (01 a 08), no formato do enunciado.
- `verificador.c`: confere a saida do programa (um unico arquivo C).
- `exemplos_esperados/`: um caminho valido (ou a mensagem de sem solucao) para cada mapa.
- `testes_verificador/`: saidas erradas feitas a mao, para testar o proprio verificador.
- `testar.ps1`: compila o verificador, roda 33 testes e grava `resultados.txt` e `resultados.csv`.
- `resultados.txt`: resultados esperados, pendencias, problema do enunciado e testes do verificador.
- `resultados.csv`: tabela das execucoes do programa principal (hoje tudo PENDENTE).

## Comandos
Rodar tudo (precisa de PowerShell e gcc):

    powershell -NoProfile -ExecutionPolicy Bypass -File .\testar.ps1

Compilar so o verificador:

    gcc -std=c99 -Wall -Wextra -Wpedantic -O0 -o verificador.exe verificador.c

Conferir uma saida do programa (use `sem_solucao` para mapas sem caminho):

    .\verificador.exe entradas\01_uma_chave.txt saida.txt com_solucao

Codigos de retorno: 0 = correta, 1 = incorreta, 2 = erro de arquivo ou mapa.

Se a compilacao falhar, o script nao apaga os resultados anteriores.

## Pendente
O programa principal ainda nao existe (leitor, busca, contadores, Makefile, modo analise).
As 16 execucoes (8 mapas x modos normal e analise), o total de chamadas recursivas
e o nivel maximo de recursao ficam PENDENTE ate la.
Quando existir, guarde cada saida real em `registros/` e preencha `resultados.csv`.
