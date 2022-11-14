# 💾 sistemas operacionais
Trabalho avaliativo para a disciplina de Sistemas Operacionais do curso de Ciência da Computação da Universidade Federal de Pelotas - RS.

## 📖 sobre
O trabalho consiste na implementação de um simulador de solicitações de crédito imobiliário com o uso de multithreading, onde cada thread corresponde a uma pessoa solicitante do dinheiro. O valor do imóvel é formado por três valores: valor em espécie, valor das taxas (5% do preço do imóvel) e subsídio fornecido pelo governo, recursos que devem ser guardados em três variáveis diferentes. É necessário que o solicitante tenha, no mínimo, 40% do valor em espécie e 50% do valor das taxas de entrada. A pessoa também já sabe quanto que tem de direito do subsídio.

Assim, quando a pessoa chega ao banco para realizar a solicitação de crédito, é preciso informar:
-	o valor do imóvel;
-	o valor das taxas;
-	quanto do dinheiro em espécie possui (mínimo 40%, máximo 70%);
-	quanto do valor das taxas (5% do valor do imóvel) possui (mínimo 50%);
-	qual a porcentagem do subsídio a qual tem direito (5%, 10% ou 20%).

Observa-se que, caso o solicitante possua o máximo permitido do valor em espécie (70%) e que tenha direito ao máximo possível de subsídio (20%), o banco precisará emprestar, sem contabilizar as taxas, no mínimo 10% do valor do imóvel.
Também fazem parte desta problemática executar ao menos três grupos de threads; fornecer um bônus governamental de 50% para o subsidio, ou seja, aumentar o valor do subsidio em 50% ao final da execução de um grupo de threads; e não deixar o programa ficar preso em um estado inseguro (informar e finalizar o programa, se for necessário).

## 🛠 implementação
Utilizou-se um semáforo mutex, de maneira que apenas bloqueava o acesso simultaneo de threads ao corpo da rotina, a qual foi dividida em blocos, utilizando-se de programação concorrente, já que, do ponto de vista do grupo, toda a região da rotina deveria ser executada por apenas um thread por vez, para não gerar inconsistências.

Também escolheu-se por deixar os parâmetros do programa na parte superior, usando a diretiva #define, a fim de facilitar a modificação desses valores, caso necessário. Cada pessoa foi representada pela estrutura Pessoa, a qual armazena o valor do imóvel gerado a partir das definições de valor mínimo e máximo associados, bem como a porcentagem que já possui de cada tipo de dinheiro (espécie, taxas e quanto tem direito ao subsidio).

A rotina da thread é dividida em cinco blocos:
1. apresenta-se os valores que a pessoa já possui e o custo do imóvel;
2. converte-se porcentagens para valores em dinheiro;
3. verificação se o banco possui os recursos necessários e, se sim, a alocação destes¹;
4. cálculo do que deve ser devolvido ao banco com os devidos juros;
5. liberação/devolução dos recursos.

>_¹ É fundamental que o teste da existência de recursos suficientes e alocação destes, em caso afirmativo, sejam realizados no mesmo bloco, pois, se feitos em partes diferentes, pode ocorrer de uma outra thread alocar os recursos e, quando a execução retornar para a thread anterior, os recursos não serem mais suficientes como anteriormente._

## 🗃 diretório
- ```trabso_v1.c:``` código do trabalho;
- ```output.txt:``` uma das muitas possíveis ordem de apresentação das mensagens geradas pela execução do trabalho; este foi gerado através do site Online GDB (mais detalhes abaixo).

## ⏯ como executar
Faz-se necessário o uso de um compilador para a linguagem C. 

Para sistemas Linux, basta compilador o código ```gcc trabso_v1.c -o -lpthread trab.exe``` e então executá-lo: ```./trab.exe```.

Para sistemas Windows, visto que o código usa threads, recomenda-se o uso de um compilador online com suporte a elas. Recomenda-se o [Online GDB](https://www.onlinegdb.com/), uma vez que ele permite inserir as flags desejadas na hora da compilação do código (neste caso, -lpthread). O site ainda aceita o upload de arquivos, bem como colocar o código diretamente nele. 

#

_Eduarda A. Carvalho, Júlia R. Junqueira, novembro/2022.<br>
eduarda.carvalho@inf.ufpel.edu.br, juliar.junqueira@inf.ufpel.edu.br_


