---
trigger: always_on
---

# Checklist de Desenho SVG: Comandos do Arquivo .qry

Este documento serve como um guia de verificação para a camada de apresentação visual do sistema. Sempre que o interpretador ler um dos comandos abaixo no arquivo `.qry`, as respectivas formas geométricas ou textos devem ser concatenados no arquivo final `.svg`.

## 🎨 Comandos com Impacto Visual no SVG

| Comando | Descrição da Ação | Elemento a ser desenhado no SVG | Status |
| :--- | :--- | :--- | :--- |
| **`rq cep`** | Remoção de Quadra | Desenhar um **"X" vermelho** na posição exata da âncora (canto sudeste) da quadra que foi removida, parar de desenhar a quadra removida no .svg gerado após consulta do qry. | [ ] Pendente |
| **`pq cep`** | Consulta de População | Escrever o **número de moradores de cada face** próximo à respectiva aresta (N, S, L, O).<br>Escrever o **total de moradores** no centro geométrico do retângulo da quadra. | [ ] Pendente |
| **`rip cpf`** | Registro de Óbito | Desenhar uma **cruz vermelha** na coordenada exata do endereço onde a pessoa residia. | [ ] Pendente |
| **`mud cpf ...`** | Mudança de Endereço | Desenhar um **quadrado vermelho** na coordenada do novo endereço.<br>Dentro deste quadrado, escrever o **CPF** do morador utilizando uma fonte minúscula. | [ ] Pendente |
| **`dspj cpf`** | Despejo de Morador | Desenhar um **círculo preto** na coordenada exata do endereço de onde o habitante foi removido. | [ ] Pendente |

---

## 📝 Comandos de Saída Exclusivamente Textual (Sem impacto no SVG)

Os comandos a seguir **não** geram nenhum desenho ou alteração no mapa visual. Eles devem apenas gerar a saída formatada no arquivo de relatório `.txt`.

| Comando | Descrição da Ação | Saída Esperada | Status |
| :--- | :--- | :--- | :--- |
| **`censo`** | Relatório Estatístico | Gravar no `.txt` os totais e percentuais de habitantes, moradores, sem-tetos e gêneros (M/F). | [ ] Pendente |
| **`h? cpf`** | Busca de Habitante | Gravar no `.txt` todos os dados pessoais do habitante (e seu endereço, caso seja um morador). | [ ] Pendente |
| **`nasc cpf ...`** | Nascimento | Apenas registrar a entidade no banco de dados (Hashfile). Sem impacto visual direto. | [ ] Pendente |

---

## ⚠️ Dicas de Implementação SVG
* Lembre-se de que o eixo Y no SVG cresce para baixo. Ajuste a posição dos textos do comando `pq` para que não fiquem fora das bordas da quadra.
* Para os desenhos de `rip`, `mud` e `dspj`, você precisará calcular a coordenada exata `(X, Y)` da casa baseando-se no ponto de ancoragem da quadra, na face escolhida e na distância (número) fornecida no endereço.
* Agrupe os desenhos gerados pelas consultas no final do arquivo `.svg`, garantindo que eles sejam renderizados "por cima" das quadras já desenhadas pelo arquivo `.geo`.