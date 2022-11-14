/*
	TRABALHO FINAL DE SISTEMAS OPERACIONAIS
		PROF. DR. RAFAEL BURLAMAQUI
		ALUNAS: EDUARDA A CARVALHO,
				    JULIA R JUNQUEIRA
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h> 
#include <pthread.h>
#include <semaphore.h>

#define N_PESSOAS 10 
#define N_GRUPOS 3

// os valores min e max definidos correspondem a:
// min --> o valor minimo do item
// max --> somados ao valor minimo, correspondem ao max total
#define VALOR_MIN_IMOVEL 100  
#define VALOR_MAX_IMOVEL 900  // max: 1000 mil
#define MIN_ESPECIE 40        
#define MAX_ESPECIE 30        // max: 70% 
#define MIN_TAXAS 50    
#define MAX_TAXAS 50          // max: 100%
#define PORCENT_TAXAS 0.05 

#define SUBSIDIO (int[]){5, 10, 20}
#define BONUS_SUBSIDIO 0.5 

#define JUROS_ESP 0.5	
#define JUROS_TAX 0.2	
#define JUROS_SUB 0.0	

#define ESPERA 1	// tempo de espera
#define ANOS_PAG 2	// anos de retorno do dinheiro
#define TRUE 1

void * rotina(void * args); 

float banco_especie = 1000;
float banco_taxas = 100; 
float banco_subsidio = 400; 
sem_t sem_binario; 

typedef struct pessoa {
  float imovel; 
  float especie;
  float taxas;
  float subsidio;
  int id;
} Pessoa; 

int main() {
  srand(time(NULL));
  pthread_t pessoas_t[N_PESSOAS];
  sem_init(&sem_binario, 0, 1); 
  Pessoa * pessoa = malloc(sizeof(Pessoa) * N_PESSOAS);
  int i, j;
  for (j = 0; j < N_GRUPOS; j++) {
    // guardando valores para futura comparação
    float especie_inicial = banco_especie;
    float taxas_inicial = banco_taxas;
    float subsidio_inicial = banco_subsidio;

    int opcoes_subsidio = sizeof(SUBSIDIO) / sizeof(int); //tamanho do vetor

    printf("\n\n#######\nGRUPO %d \n  --- \nINICIO\n#######\n\n", j+1);
	  for (i = 0; i < N_PESSOAS; i++) { 
      // obtendo valor do imovel (em mil reais) e dos outros valores (%)
      (pessoa + i)->imovel = (rand() % VALOR_MAX_IMOVEL) + VALOR_MIN_IMOVEL;
      (pessoa + i)->especie = (rand() % MAX_ESPECIE) + MIN_ESPECIE;
      (pessoa + i)->taxas = (rand() % MAX_TAXAS) + MIN_TAXAS; 
      (pessoa + i)->subsidio = SUBSIDIO[rand() % opcoes_subsidio];
      (pessoa + i)->id = i;

      if (pthread_create(&pessoas_t[i], NULL, rotina, (pessoa + i)) != 0)
        perror("Criacao da thread falhou.\n");
      }

      for (i = 0; i < N_PESSOAS; i++) {
        if (pthread_join(pessoas_t[i], NULL) != 0)
          perror("Juncao da thread falhou.\n");
      }

      printf("\n\n#######\nGRUPO %d\n  --- \n  FIM \n#######\n", j+1);
      printf("\tO banco possuia: %.0f mil em especie, %.0f em taxas e %.0f em subsidio.\n", especie_inicial, taxas_inicial, subsidio_inicial);
      printf("\tAgora tem: %.0f mil em especie, %.0f em taxas e %.0f em subsidio.\n", banco_especie, banco_taxas, banco_subsidio);
      banco_subsidio = banco_subsidio + (banco_subsidio * BONUS_SUBSIDIO);
      printf("\tCom acrescimo de 50%% de subsidio, o banco tem agora %.0f mil de subsidio.\n", banco_subsidio);
   }
   
  printf("\n\nForam atendidos %d grupos de %d pessoas no banco.",N_GRUPOS, N_PESSOAS);
  printf("\nEncerrando o expediente.");
  free(pessoa);
  sem_destroy(&sem_binario);
  return 0;
}

void * rotina(void * args) {
  Pessoa * pessoa;
  int cem_porcent = 100; //auxiliar
  
  sem_wait(&sem_binario); // BLOCO 1: apresenta porcentagens
  pessoa = (Pessoa *)args;
  float taxas = ceil(pessoa->imovel * PORCENT_TAXAS);
  printf("\nPESSOA %d -- custo imovel: %.0f mil - taxas: %.0f mil.\n", pessoa->id, pessoa->imovel, taxas);
  printf("\t    tem %.0f%% em especie e ganhou %.0f%% de subsidio. Possui %.0f%% das taxas.\n", pessoa->especie, pessoa->subsidio, pessoa->taxas);
  float precisa_especie = cem_porcent - pessoa->especie - pessoa->subsidio;
  float precisa_taxas = cem_porcent - pessoa->taxas;
  printf("\t    precisa de %.0f%% em especie e %.0f%% das taxas.\n", precisa_especie, precisa_taxas);
  printf("\tEsperando aprovacao de credito.\n");
  sem_post(&sem_binario);
  sleep(ESPERA);

  sem_wait(&sem_binario); // BLOCO 2: calcula valores e verifica se o banco tem
  float valor_especie = pessoa->imovel * (precisa_especie / cem_porcent);
  float valor_taxas = taxas * (precisa_taxas / cem_porcent);
  float valor_subsidio = pessoa->imovel * (pessoa->subsidio / cem_porcent);
  sem_post(&sem_binario);
  sleep(ESPERA);
  
  while (TRUE) {
	  sem_wait(&sem_binario); // BLOCO 3: verificação e alocação
	  if (banco_especie - valor_especie < 0 || banco_taxas - valor_taxas < 0 || banco_subsidio - valor_subsidio < 0) {
		printf("\nPESSOA %d -- Banco sem recursos suficientes (estado inseguro, credito negado).\n",pessoa->id);
		// printf("\tPediu -> especie: %.0f mil - taxas: %.0f mil - subsidio: %.0f mil\n", valor_especie, valor_taxas, valor_subsidio);
		// printf("\tBanco -> especue: %.0f mil - taxas: %.0f mil - subsidio: %.0f mil\n", banco_especie, banco_taxas, banco_taxas);
		sem_post(&sem_binario);
		sleep(ESPERA); 
	  } else break;
  }
  printf("\nPESSOA %d -- credito aprovado. O banco emprestou:\n", pessoa->id);
  printf("\t    %.0f mil em especie;\n\t    %.0f mil das taxas;\n\t    %.0f mil de subsidio.\n", valor_especie, valor_taxas, valor_subsidio);
  banco_especie -= valor_especie;
  banco_taxas -= valor_taxas;
  banco_subsidio -= valor_subsidio;
  printf("O banco possui agora: %.0f mil em especie -- %.0f mil em taxas -- %.0f mil em subsidio.\n", banco_especie, banco_taxas, banco_subsidio);
  sem_post(&sem_binario);
  sleep(ESPERA);

	sem_wait(&sem_binario); // BLOCO 4: calculos de devolução
	float * aux = malloc(sizeof(float));
	*aux = valor_especie/ANOS_PAG;
	float parte_especie = (*aux) + ((*aux) * JUROS_ESP);
	*aux = valor_taxas/ANOS_PAG;
	float parte_taxas = (*aux) + ((*aux) * JUROS_TAX);
	*aux = valor_subsidio/ANOS_PAG;
	float parte_subsidio = (*aux) + ((*aux) * JUROS_SUB);
	free(aux);
	sem_post(&sem_binario);
  sleep(ESPERA);
  
  for (int j = 0; j < ANOS_PAG; j++) { 
    sleep(ESPERA);
    sem_wait(&sem_binario); // BLOCO 5: devolução
    banco_especie += parte_especie;
    banco_taxas += parte_taxas;
    banco_subsidio += parte_subsidio;
    printf("\nPESSOA %d -- devolveu dinheiro durante um ano (%d/%d).\n", pessoa->id, j + 1, ANOS_PAG);
    printf("O banco possui agora: %.0f mil em especie -- %.0f mil em taxas -- %.0f mil em subsidio.\n", banco_especie, banco_taxas, banco_subsidio);
    sem_post(&sem_binario); 
  }
}