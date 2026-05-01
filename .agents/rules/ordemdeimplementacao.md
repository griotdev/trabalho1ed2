---
trigger: always_on
---

# Ordem de Implementação: Sistema de Informação Geográfica (SIG)

Esta é a esteira de desenvolvimento recomendada para maximizar a eficiência e garantir que as dependências do projeto sejam resolvidas na ordem correta, minimizando falhas de segmentação e refatorações tardias.

## Fase 1: Infraestrutura e Configuração Base
* [cite_start]Criar a estrutura de diretórios obrigatória: raiz, `/src` e `/tst`[cite: 61].
* [cite_start]Configurar o arquivo `makefile` na raiz do projeto[cite: 63].
* [cite_start]O `makefile` deve possuir a flag de compilação `-std=c99`[cite: 67].
* [cite_start]O `makefile` deve obrigatoriamente usar a flag `-fstack-protector-all`[cite: 64].
* [cite_start]Configurar *targets* no `makefile` para gerar os objetos, o executável final e rodar os testes unitários (`tstall`)[cite: 63, 65, 66].
* [cite_start]Integrar os arquivos do framework Unity (`unity.c`, `unity.h`, `unity_internals.h`) para os testes unitários[cite: 155, 202].

## Fase 2: Interface de Linha de Comando (CLI)
* [cite_start]Implementar a função `main` no arquivo raiz para gerar o executável chamado `ted`[cite: 63, 107].
* [cite_start]Criar o *parser* de argumentos para capturar o diretório de entrada (`-e`), arquivo de descrição (`-f`), arquivo de pessoas/moradores (`-pm`), arquivo de consultas (`-q`) e diretório de saída (`-o`)[cite: 157, 158, 160, 161, 162, 163].

## Fase 3: Núcleo de Armazenamento (Hashfile Dinâmico)
* [cite_start]Desenvolver o cabeçalho (`.h`) do Hashfile Dinâmico em disco, lembrando de aplicar o conceito de ponteiros opacos (sem expor a `struct`)[cite: 31, 39].
* [cite_start]Escrever os testes unitários rigorosos para a manipulação do arquivo binário `.hf` antes da implementação[cite: 198, 199, 200, 201].
* [cite_start]Implementar as funções em C para inserção, busca e expansão dos *buckets* em disco[cite: 283, 284, 285].
* [cite_start]Criar a função que gera o despejo textual do Hashfile, que deverá ter a extensão `.hfd` informando o esquema do arquivo e expansões[cite: 33, 34].

## Fase 4: Entidades de Negócio (Bitnópolis)
* [cite_start]Criar os módulos (TADs) para representar as Quadras, contendo CEP, coordenadas (x, y), largura (w) e altura (h)[cite: 15].
* [cite_start]Criar os módulos para Habitantes (CPF, nome, sobrenome, sexo, nascimento) e Moradores (endereço atrelado a um Habitante)[cite: 18].
* [cite_start]Vincular os dados destas entidades aos Hashfiles instanciados (provavelmente 2 Hashfiles distintos para quadras e habitantes)[cite: 34].

## Fase 5: Interpretadores de Comandos (Parsers)
* [cite_start]Implementar a leitura iterativa linha a linha do arquivo `.geo` para instanciar a cidade[cite: 84].
* [cite_start]Implementar a leitura do arquivo `.pm` para povoar o sistema com as pessoas e conectá-las aos endereços[cite: 18].
* [cite_start]Implementar o motor de interpretação do arquivo `.qry`, executando remoções, cálculos de censo e mudanças de endereço[cite: 18, 19, 20, 21].

## Fase 6: Camada de Apresentação (Saídas SVG e TXT)
* [cite_start]Desenvolver o módulo de geração de gráficos vetoriais `.svg` para desenhar os retângulos (quadras) com cores de preenchimento e borda[cite: 15, 98].
* [cite_start]Adicionar lógicas visuais de anotação no `.svg` com base nos comandos `.qry` (ex: X vermelho ao remover quadra, cruz vermelha ao registrar falecimento)[cite: 19, 27].
* [cite_start]Gerar o relatório final em `.txt` contendo as respostas processadas dos comandos de consulta, precedidas por `[*]`[cite: 104, 105].