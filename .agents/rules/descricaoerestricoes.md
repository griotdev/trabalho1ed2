---
trigger: always_on
---

# Documento de Requisitos e Restrições: Projeto SIG (Bitnópolis)

Este documento define o escopo, as entidades principais e as regras rigorosas de avaliação que devem ser respeitadas durante a codificação do Sistema de Informações Geográficas (SIG).

## Visão Geral do Sistema
[cite_start]O sistema irá modelar uma cidade fictícia chamada Bitnópolis, composta por quadras retangulares[cite: 6, 7]. [cite_start]A cidade possui um sistema de endereçamento no formato `CEP/Face/número`[cite: 12]. [cite_start]A população é dividida em Habitantes e Moradores (habitantes com endereço fixo; os sem-teto são habitantes que não são moradores)[cite: 14, 17]. 

## Regra Crítica de Avaliação (Evitando Nota Zero ou Descontos)
| Restrição Técnica | Penalidade em caso de quebra |
| :--- | :--- |
| [cite_start]**Ponteiros Opacos Obrigatórios:** É estritamente proibido definir a estrutura interna da `struct` no arquivo `.h`[cite: 39]. | [cite_start]Desconto de 2.5 pontos[cite: 39]. |
| [cite_start]**Erro de Compilação:** O código deve compilar perfeitamente via `makefile`[cite: 44]. | [cite_start]Nota ZERO[cite: 44]. |
| [cite_start]**Falta de Testes Unitários:** Todo módulo deve possuir seu próprio arquivo de teste `t_m.c` utilizando o framework Unity[cite: 42, 62]. | [cite_start]Desconto de até 2.5 pontos[cite: 42]. |
| [cite_start]**Não implementar Hashfile em Disco:** A persistência principal das quadras e habitantes deve ser feita no `.hf`[cite: 31, 32]. | [cite_start]Desconto grave ou total[cite: 39]. |
| [cite_start]**C99 e Proteção de Pilha:** Compilar os fontes sem `-std=c99` ou `-fstack-protector-all`[cite: 47, 48]. | [cite_start]Penalização por não seguir os padrões técnicos[cite: 47, 48]. |

## Arquivos de Entrada e Processamento
* [cite_start]**.geo:** Define as formas da cidade e cria as entidades primárias, como as quadras (`c`, `q`, `cq`)[cite: 15, 84].
* [cite_start]**.pm:** Insere os habitantes (`p`) e os cadastra como moradores em endereços específicos (`m`)[cite: 18].
* [cite_start]**.qry:** Executa consultas dinâmicas ou modificações, como remoção de quadras (`rq`), censo detalhado (`censo`), nascimento/óbito (`nasc`, `rip`), mudança de endereço (`mud`) e despejo (`dspj`)[cite: 18, 19, 21, 25, 27, 30].

## Padrões de Saída
* [cite_start]**Arquivo .hf:** O arquivo binário onde os registros do Hashfile Extensível/Dinâmico são mantidos em disco[cite: 31, 32].
* [cite_start]**Arquivo .hfd:** Uma representação legível em arquivo-texto do conteúdo e das expansões do Hashfile (`.hf`)[cite: 33, 34].
* [cite_start]**Arquivo .svg:** Representação visual do banco de dados ao final de todo o processamento das consultas, contendo polígonos, cores e indicadores visuais (ex: quadrados vermelhos, cruzes e textos com estatísticas)[cite: 19, 21, 27, 28, 103].
* **Arquivo .txt:** Log das consultas de processamento do `.qry`. [cite_start]A linha do comando deve ser exibida precedida por `[*]` e, nas linhas seguintes, o respectivo resultado textual (ex: relatórios estatísticos do censo ou dados de uma pessoa)[cite: 22, 24, 104, 105, 106].