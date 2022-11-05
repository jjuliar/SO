#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h> //sleep()
#include <pthread.h>
#include <semaphore.h>

#define N_PESSOAS 10 
#define N_GRUPOS 3

// os valores min e max definidos correspondem a:
// min --> o valor minimo do item
// max --> somados ao valor minimo, correspondem ao max total
#define VALOR_MIN_IMOVEL 100  // min 100 mil
#define VALOR_MAX_IMOVEL 900  // max 1000 mil
#define MIN_ESPECIE 40        // min 40% 
#define MAX_ESPECIE 30        // max 70% 
#define MIN_TAXAS 50          // min 50% 
#define MAX_TAXAS 50          // max 100%
#define PORCENT_TAXAS 0.05    // 5% do valor do imovel
#define OPCOES_SUBSIDIO 3
#define SUBSIDIO (int[]){5, 10, 20}

#define JUROS_ESP 0.5	// juros do banco para valor em especie
#define JUROS_TAX 0.2	// juros do banco para valor das taxas
#define JUROS_SUB 0.0	// juros do banco para valor do subsidio

#define ESPERA 1	// tempo de espera
#define TEMPO_PAG 5	// anos de retorno do dinheiro

void * rotina(void * args); // prototipo da funcao da thread

sem_t sem_binario;

float banco_especie = 1000;
float banco_taxas = 100; // 10% de 1000 (dobro do max possivel)
float banco_subsidio = 400; // 40% de 1000 mil (dobro do max possivel)

typedef struct pessoa {
  float imovel; 
  float especie;
  float taxas;
  float subsidio;
  int id;
} Pessoa; // valores relacionados ao que a pessoa quer (imovel) ou tem (valores)

int main() {
  srand(time(NULL));
  pthread_t pessoas_t[N_PESSOAS];
  // iniciliza o semaforo com tres argumentos: indica a variavel
  // se tem "only multi thread, pass 0; multi processes, pass 1"
  // e valor inicial do semaforo (1 pois exclusao mutua?)
  sem_init(&sem_binario, 0, 1); 
  Pessoa * pessoa = malloc(sizeof(Pessoa) * N_PESSOAS);
  int i;
  //int j;
  //for (j = 0; j < N_GRUPOS; j++) {
	  for (i = 0; i < N_PESSOAS; i++) {
		// obtendo valor do imovel (em mil reais) e a porcentagem dos outros valores
		(pessoa + i)->imovel = (rand() % VALOR_MAX_IMOVEL) + VALOR_MIN_IMOVEL;
		(pessoa + i)->especie = (rand() % MAX_ESPECIE) + MIN_ESPECIE;
		(pessoa + i)->taxas = (rand() % MAX_TAXAS) + MIN_TAXAS; 
		(pessoa + i)->subsidio = SUBSIDIO[rand() % OPCOES_SUBSIDIO];
		(pessoa + i)->id = i;
		if (pthread_create(&pessoas_t[i], NULL, rotina, (pessoa + i)) != 0)
		  perror("Criacao da thread falhou.\n");
	  }
	  for (i = 0; i < N_PESSOAS; i++) {
		if (pthread_join(pessoas_t[i], NULL) != 0)
		  perror("Juncao da thread falhou.\n");
	  }
  //  }
  free(pessoa);
  sem_destroy(&sem_binario);
  return 0;
}

void * rotina(void * args) {
  Pessoa * pessoa;
  
  sem_wait(&sem_binario); // apresenta porcentagens
  pessoa = (Pessoa *)args;
  float taxas = ceil(pessoa->imovel * PORCENT_TAXAS);
  printf("\nPESSOA %d -- custo imovel: %.0f mil - taxas: %.0f mil.\n", pessoa->id, pessoa->imovel, taxas);
  printf("\t    tem %.0f%% em especie e ganhou %.0f%% de subsidio. Possui %.0f%% das taxas.\n", pessoa->especie, pessoa->subsidio, pessoa->taxas);
  printf("\t    precisa de %.0f%% em especie e %.0f%% das taxas.\n", (100 - pessoa->especie - pessoa->subsidio), (100 - pessoa->taxas));
  printf("\tEsperando aprovacao de credito.\n");
  sem_post(&sem_binario);

  sleep(ESPERA);

  sem_wait(&sem_binario); // calcula valores com porcent e verifica se o banco tem
  int cem_porcent = 100;
  float valor_especie = pessoa->imovel * ((cem_porcent - pessoa->especie - pessoa->subsidio) / cem_porcent);
  float valor_taxas = taxas * ((cem_porcent - pessoa->taxas) / cem_porcent);
  float valor_subsidio = pessoa->imovel * (pessoa->subsidio / cem_porcent);
  while (!(banco_especie - valor_especie > 0 && banco_taxas - valor_taxas > 0 && banco_subsidio - valor_subsidio > 0)) {
    printf("\nPESSOA %d -- Banco sem recursos.\n",pessoa->id);
    printf("\tPediu esp: %.0f mil - tax: %.0f mil - sub: %.0f mil\n", valor_especie, valor_taxas, valor_subsidio);
    printf("\tBanco esp: %.0f mil - tax: %.0f mil - sub: %.0f mil\n", banco_especie, banco_taxas, banco_taxas);
    sem_post(&sem_binario);
    sleep(ESPERA); 
  }
  sem_post(&sem_binario);
  
  sem_wait(&sem_binario); // retira valores do banco
  printf("\nPESSOA %d -- credito aprovado. O banco emprestou:\n", pessoa->id);
  printf("\t    %.0f mil em especie;\n\t    %.0f mil das taxas;\n\t    %.0f mil de subsidio.\n", valor_especie, valor_taxas, valor_subsidio);
  banco_especie -= valor_especie;
  banco_taxas -= valor_taxas;
  banco_subsidio -= valor_subsidio;
  printf("O banco possui agora: %.0f mil em especie -- %.0f mil em taxas -- %.0f mil em subsidio.\n", banco_especie, banco_taxas, banco_subsidio);
  sem_post(&sem_binario);

  for (int j = 0; j < TEMPO_PAG; j++) { // devolve valores ao banco
    sleep(ESPERA);
    sem_wait(&sem_binario); 
    banco_especie += (valor_especie/TEMPO_PAG) + ((valor_especie/TEMPO_PAG) * JUROS_ESP);
    banco_taxas += (valor_taxas/TEMPO_PAG) + ((valor_taxas/TEMPO_PAG) * JUROS_TAX);
    banco_subsidio += (valor_subsidio/TEMPO_PAG) + ((valor_subsidio/TEMPO_PAG) * JUROS_SUB);
    printf("\nPESSOA %d -- devolveu dinheiro durante um ano (%d/%d).\n", pessoa->id, j + 1, TEMPO_PAG);
    printf("O banco possui agora: %.0f mil em especie -- %.0f mil em taxas -- %.0f mil em subsidio.\n", banco_especie, banco_taxas, banco_subsidio);
    sem_post(&sem_binario); 
  }
}