# Executar: powershell -NoProfile -ExecutionPolicy Bypass -File .\testar.ps1
# O script compila somente o verificador e grava resultados.txt e resultados.csv.
param([string]$Compilador = 'gcc')

$ErrorActionPreference = 'Stop'

# Teste, resultado esperado do labirinto e motivo.
$mapas = @(
@('01_uma_chave', 'com_solucao', 'O corredor permite coletar a chave e chegar ao bau.'),
@('02_varias_chaves', 'com_solucao', 'Ha caminhos sem repeticao que coletam as tres chaves.'),
@('03_becos_sem_saida', 'com_solucao', 'A linha 1 leva ao bau; cima e baixo em [1,2] sao becos. O retorno depende da ordem de exploracao.'),
@('04_bau_inacessivel', 'sem_solucao', 'O bau esta isolado por paredes e pelas bordas.'),
@('05_chave_inacessivel', 'sem_solucao', 'O bau e alcancavel, mas uma linha de paredes isola a chave.'),
@('06_exige_repeticao', 'sem_solucao', 'Sair dos ramos com chaves exigiria repetir a sala [1,1].'),
@('07_apenas_diagonal', 'sem_solucao', 'A esta bloqueado nas quatro direcoes; a diagonal e proibida.'),
@('08_corredor_longo', 'com_solucao', 'As duas chaves ficam no corredor antes do bau. A recursao ainda precisa ser medida.')
)

# Nome, mapa, resposta de teste, tipo esperado, codigo esperado e mensagem.
# Mensagem vazia: conferir apenas o codigo de saida.
$casos = @(
@('01_uma_chave', 'entradas/01_uma_chave.txt', 'exemplos_esperados/01_uma_chave.txt', 'com_solucao', '0', ''),
@('02_varias_chaves', 'entradas/02_varias_chaves.txt', 'exemplos_esperados/02_varias_chaves.txt', 'com_solucao', '0', ''),
@('03_becos_sem_saida', 'entradas/03_becos_sem_saida.txt', 'exemplos_esperados/03_becos_sem_saida.txt', 'com_solucao', '0', ''),
@('04_bau_inacessivel', 'entradas/04_bau_inacessivel.txt', 'exemplos_esperados/04_bau_inacessivel.txt', 'sem_solucao', '0', ''),
@('05_chave_inacessivel', 'entradas/05_chave_inacessivel.txt', 'exemplos_esperados/05_chave_inacessivel.txt', 'sem_solucao', '0', ''),
@('06_exige_repeticao', 'entradas/06_exige_repeticao.txt', 'exemplos_esperados/06_exige_repeticao.txt', 'sem_solucao', '0', ''),
@('07_apenas_diagonal', 'entradas/07_apenas_diagonal.txt', 'exemplos_esperados/07_apenas_diagonal.txt', 'sem_solucao', '0', ''),
@('08_corredor_longo', 'entradas/08_corredor_longo.txt', 'exemplos_esperados/08_corredor_longo.txt', 'com_solucao', '0', ''),
@('alternativo', 'entradas/02_varias_chaves.txt', 'testes_verificador/caminho_alternativo.txt', 'com_solucao', '0', ''),
@('linhas_separadas', 'entradas/01_uma_chave.txt', 'testes_verificador/coordenadas_em_linhas.txt', 'com_solucao', '0', ''),
@('inicio_errado', 'entradas/01_uma_chave.txt', 'testes_verificador/inicio_errado.txt', 'com_solucao', '1', ''),
@('fim_errado', 'entradas/01_uma_chave.txt', 'testes_verificador/fim_errado.txt', 'com_solucao', '1', ''),
@('fora_do_mapa', 'entradas/01_uma_chave.txt', 'testes_verificador/fora_do_mapa.txt', 'com_solucao', '1', ''),
@('coordenada_negativa', 'entradas/01_uma_chave.txt', 'testes_verificador/coordenada_negativa.txt', 'com_solucao', '1', ''),
@('salto', 'entradas/01_uma_chave.txt', 'testes_verificador/salto.txt', 'com_solucao', '1', ''),
@('parede', 'entradas/04_bau_inacessivel.txt', 'testes_verificador/parede.txt', 'com_solucao', '1', ''),
@('repeticao', 'entradas/01_uma_chave.txt', 'testes_verificador/repeticao.txt', 'com_solucao', '1', ''),
@('diagonal', 'entradas/07_apenas_diagonal.txt', 'testes_verificador/diagonal.txt', 'com_solucao', '1', ''),
@('faltam_chaves', 'entradas/02_varias_chaves.txt', 'testes_verificador/faltam_chaves.txt', 'com_solucao', '1', ''),
@('falsa_ausencia', 'entradas/01_uma_chave.txt', 'testes_verificador/falsa_ausencia.txt', 'com_solucao', '1', 'FALHA: era esperado um caminho, mas foi informada ausencia de solucao.'),
@('mensagem_errada', 'entradas/04_bau_inacessivel.txt', 'testes_verificador/mensagem_errada.txt', 'sem_solucao', '1', ''),
@('mensagem_e_caminho', 'entradas/01_uma_chave.txt', 'testes_verificador/mensagem_e_caminho.txt', 'com_solucao', '1', 'FALHA: era esperado um caminho, mas foi informada ausencia de solucao.'),
@('coordenada_malformada', 'entradas/01_uma_chave.txt', 'testes_verificador/coordenada_malformada.txt', 'com_solucao', '1', ''),
@('saida_vazia', 'entradas/01_uma_chave.txt', 'testes_verificador/saida_vazia.txt', 'com_solucao', '1', ''),
@('caminho_no_caso_sem_solucao', 'entradas/04_bau_inacessivel.txt', 'testes_verificador/parede.txt', 'sem_solucao', '1', 'FALHA: a mensagem exigida de ausencia de solucao nao apareceu.'),
@('mapa_com_chaves_erradas', 'testes_verificador/mapa_com_chaves_erradas.txt', 'exemplos_esperados/01_uma_chave.txt', 'com_solucao', '2', ''),
@('arquivo_saida_ausente', 'entradas/01_uma_chave.txt', 'testes_verificador/arquivo_que_nao_existe.txt', 'com_solucao', '2', ''),
@('exemplo_do_enunciado', 'testes_verificador/enunciado_mapa.txt', 'testes_verificador/enunciado_saida.txt', 'com_solucao', '1', ''),
@('cabecalho_incompleto', 'testes_verificador/cabecalho_incompleto.txt', 'exemplos_esperados/01_uma_chave.txt', 'com_solucao', 2, 'ERRO: cabecalho incompleto ou com valores invalidos.'),
@('dimensoes_invalidas', 'testes_verificador/dimensoes_invalidas.txt', 'exemplos_esperados/01_uma_chave.txt', 'com_solucao', 2, 'ERRO: dimensoes devem estar entre 1 e 100.'),
@('mapa_grande', 'testes_verificador/mapa_grande.txt', 'exemplos_esperados/01_uma_chave.txt', 'com_solucao', 2, 'ERRO: dimensoes devem estar entre 1 e 100.'),
@('chaves_em_excesso', 'testes_verificador/chaves_em_excesso.txt', 'exemplos_esperados/01_uma_chave.txt', 'com_solucao', 2, 'ERRO: quantidade de chaves invalida para as dimensoes do mapa.'),
@('chaves_negativas', 'testes_verificador/chaves_negativas.txt', 'exemplos_esperados/01_uma_chave.txt', 'com_solucao', 2, 'ERRO: quantidade de chaves invalida para as dimensoes do mapa.')
)

$registro = @(
    'TESTES DA PESSOA 4 - VERSAO BASICA',
    '',
    'Comando para repetir:',
    'powershell -NoProfile -ExecutionPolicy Bypass -File .\testar.ps1',
    ('Comando de compilacao no terminal: ' + (Split-Path -Leaf $Compilador) + ' -std=c99 -Wall -Wextra -Wpedantic -O0 -o verificador.exe verificador.c'),
    'Uso manual: .\verificador.exe mapa.txt saida.txt com_solucao|sem_solucao',
    'Coordenadas [linha,coluna], a partir de zero; arquivos em UTF-8.',
    'Os exemplos_esperados sao respostas manuais. Qualquer caminho valido e aceito.',
    '',
    'RESULTADOS ESPERADOS DOS MAPAS'
)
foreach ($mapa in $mapas) {
    $registro += 'entradas/' + $mapa[0] + '.txt | ' + $mapa[1] + ' | ' + $mapa[2]
}

$registro += @(
    '',
    'PENDENCIAS DO PROGRAMA PRINCIPAL',
    'Ainda faltam leitor (pessoa 1), busca e contadores (pessoa 2), integracao e comandos (pessoa 3).',
    'Os comandos de entrada, execucao normal e ativacao da analise ainda nao foram fornecidos.',
    'No teste 03, conferir o abandono dos becos quando a ordem de movimentos estiver definida.'
)

$registro += @(
    '',
    'PROBLEMA ENCONTRADO NO ENUNCIADO',
    'A saida do exemplo do mapa 10x10 (Figura 2) contradiz as regras escritas.',
    'Repete [2,3] a [2,6], [0,7] a [0,9] e [1,9] a [5,9]: aparecem duas vezes.',
    'Passa pelas paredes [1,7] e [8,9] e nao coleta a chave [8,0].',
    'O verificador rejeita no passo 26, em [2,3] (teste exemplo_do_enunciado).',
    'Arquivos em testes_verificador/: enunciado_mapa.txt e enunciado_saida.txt.',
    'Conferencia exaustiva separada: 6 caminhos validos; o menor tem 43 salas.',
    'Decisao: seguir as regras escritas: nao repetir sala, nao atravessar parede,',
    'coletar todas as chaves e confirmar com o professor.'
)

$registro += @(
    '',
    'Teste | Modo | Esperado | Obtido | Status | Chamadas recursivas | Nivel maximo'
)
foreach ($mapa in $mapas) {
    foreach ($modo in @('normal', 'analise')) {
        $registro += $mapa[0] + ' | ' + $modo + ' | ' + $mapa[1] +
            ' | PENDENTE | PENDENTE | PENDENTE | PENDENTE'
    }
}

$registro += @(
    '',
    'TESTES DO VERIFICADOR',
    'Respostas incorretas devem ser rejeitadas: codigo 1. Erros de arquivo/mapa: codigo 2.',
    'Codigo 0 aprova a resposta. O status abaixo indica se o teste do verificador passou.',
    'Teste | Codigo esperado | Codigo obtido | Status | Saida real'
)

# Tabela em CSV: execucoes do programa principal, ainda pendentes.
$csv = @('teste,arquivo_entrada,resultado_esperado,modo,resultado_obtido,aprovado,chamadas_recursivas,nivel_maximo')
foreach ($mapa in $mapas) {
    foreach ($modo in @('normal', 'analise')) {
        $csv += $mapa[0] + ',entradas/' + $mapa[0] + '.txt,' + $mapa[1] + ',' + $modo +
            ',PENDENTE,PENDENTE,PENDENTE,PENDENTE'
    }
}

$codigoScript = 2
$gravar = $false
Push-Location $PSScriptRoot
try {
    $preferenciaAnterior = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try {
        $compilacao = & $Compilador -std=c99 -Wall -Wextra -Wpedantic -O0 -o verificador.exe verificador.c 2>&1
    } finally {
        $ErrorActionPreference = $preferenciaAnterior
    }
    if (($compilacao -join '').Trim() -ne '') {
        $registro += $compilacao
    }
    if ($LASTEXITCODE -ne 0) {
        throw ('Falha na compilacao: ' + ($compilacao -join ' '))
    }
    $falhas = 0
    foreach ($caso in $casos) {
        $saida = & .\verificador.exe $caso[1] $caso[2] $caso[3]
        $codigo = $LASTEXITCODE
        $mensagem = ($saida -join ' ').Trim()
        $passou = $codigo -eq [int]$caso[4]
        if ($caso[5] -ne '' -and $mensagem -ne $caso[5]) {
            $passou = $false
        }
        if ($passou) {
            $status = 'APROVADO'
        } else {
            $status = 'FALHA'
            $falhas++
        }
        $registro += $caso[0] + ' | ' + $caso[4] + ' | ' + $codigo +
            ' | ' + $status + ' | ' + $mensagem
    }
    $registro += 'Total: ' + $casos.Count + '; aprovados: ' + ($casos.Count - $falhas) + '; falhas: ' + $falhas
    if ($registro -match '([A-Za-z]:[\\/]|\\\\|(^|\s)/)') {
        throw 'O registro contem caminho absoluto; resultados anteriores preservados.'
    }
    $gravar = $true
    if ($falhas -eq 0) {
        $codigoScript = 0
    } else {
        $codigoScript = 1
    }
} catch {
    $gravar = $false
    $codigoScript = 2
    Write-Output ('Execucao interrompida: ' + $_.Exception.Message)
    Write-Output 'Os resultados anteriores foram preservados.'
} finally {
    if ($gravar) {
        Set-Content -LiteralPath (Join-Path $PSScriptRoot 'resultados.txt') -Value $registro -Encoding ASCII
        Set-Content -LiteralPath (Join-Path $PSScriptRoot 'resultados.csv') -Value $csv -Encoding ASCII
    }
    Pop-Location
}
$registro | ForEach-Object { Write-Output $_ }
exit $codigoScript
