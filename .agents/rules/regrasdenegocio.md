---
trigger: always_on
---

# Regras de Negócio: Sistema de Informação Geográfica (SIG)

Este documento detalha a lógica de domínio de Bitnópolis, o funcionamento do endereçamento e as assinaturas de todos os comandos que devem ser suportados pelos *parsers*.

## 1. Sistema de Coordenadas e Endereçamento
* [cite_start]**Ponto de Ancoragem:** O ponto base para os cálculos de uma quadra (retângulo) é sempre o seu canto sudeste[cite: 246].
* [cite_start]**Formato do Endereço:** Os endereços seguem o modelo `CEP/Face/número`, por exemplo: `cep15/S/45`[cite: 246].
* [cite_start]**Cálculo Numérico:** O número de uma casa é a distância da frente da casa até a projeção do ponto de ancoragem da respectiva face[cite: 244, 245].
* [cite_start]**Habitantes vs Moradores:** Todo morador é um habitante, mas os sem-teto são habitantes que não possuem endereço (não são moradores)[cite: 251].

## 2. Padrão de Nomeação de Arquivos de Saída
* [cite_start]Ao processar um arquivo `.geo` e um `.qry` em conjunto, os arquivos finais devem combinar os nomes[cite: 126]. [cite_start]Por exemplo, lendo `arq.geo` e `arqcons.qry`, os arquivos gerados serão `arq-arqcons.svg` e `arq-arqcons.txt`[cite: 143].
* [cite_start]No arquivo `.txt`, cada linha de consulta executada deve ser impressa precedida por `[*] ` e o resultado deve vir logo nas linhas seguintes[cite: 128, 129, 130].

## 3. Comandos do Arquivo de Cidade (.geo)
* [cite_start]`q cep x y w h`: Insere uma quadra (retângulo) identificada pelo `cep`[cite: 249].
* [cite_start]`cq sw cfill cstrk`: Define a espessura da borda (`sw`), a cor de preenchimento (`cfill`) e a cor da borda (`cstrk`) para as próximas quadras lidas[cite: 249].

## 4. Comandos do Arquivo de População (.pm)
* [cite_start]`p cpf nome sobrenome sexo nasc`: Insere um habitante no sistema[cite: 252].
* [cite_start]`m cpf cep face num compl`: Atribui um endereço a um habitante existente, tornando-o um morador[cite: 252].

## 5. Comandos do Arquivo de Consultas (.qry)
* `rq cep`: Remove a quadra especificada. [cite_start]Moradores dessa quadra perdem o endereço e tornam-se sem-teto[cite: 253]. [cite_start]No `.txt`, relatar CPF e nome dos afetados[cite: 253]. [cite_start]No `.svg`, desenhar um "X" vermelho na posição da âncora da quadra removida[cite: 253].
* `pq cep`: Consulta populacional da quadra. [cite_start]No `.svg`, escrever o número de moradores de cada face próximo ao seu limite, e o total de moradores no centro da quadra[cite: 254, 255].
* [cite_start]`censo`: Gera relatório estatístico no `.txt`[cite: 256]. [cite_start]Incluir totais e percentuais de habitantes, moradores, gêneros (M/F) e sem-tetos[cite: 256].
* `h? cpf`: Busca de pessoa. [cite_start]No `.txt`, exibir todos os dados do habitante[cite: 258]. [cite_start]Se for morador, incluir também o endereço atual[cite: 259].
* [cite_start]`nasc cpf nome sobrenome sexo nasc`: Registra o nascimento de um novo habitante[cite: 259].
* `rip cpf`: Registra o óbito de uma pessoa. [cite_start]No `.txt`, imprimir os dados do falecido (e endereço, se aplicável)[cite: 259, 260]. [cite_start]No `.svg`, desenhar uma cruz vermelha no local do endereço[cite: 261].
* `mud cpf cep face num cmpl`: O morador muda de endereço. [cite_start]No `.svg`, marcar o novo endereço com um quadrado vermelho contendo o CPF dentro (em fonte minúscula)[cite: 261, 262, 263].
* `dspj cpf`: Despeja o morador, tornando-o sem-teto. [cite_start]No `.txt`, relatar os dados da pessoa e o endereço de onde foi removida[cite: 264]. [cite_start]No `.svg`, desenhar um círculo preto na coordenada do despejo[cite: 264, 265].