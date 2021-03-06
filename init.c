#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <argp.h>
#include "header.h"


// Instancia as variáveis globais
unsigned int		ROUTER_ID;
unsigned int 		TRANSM_TIMEOUT;
unsigned int 		TRANSM_MAX_ATTEMPTS;
unsigned int 		TRANSM_USLEEP_TIME;
unsigned int 		INTERFRAME_DELAY;
short unsigned int 	LAST_SENT_DGRAM_ID;
unsigned long		TR_SUCCESS;
unsigned long		TR_WARNING;
unsigned long		TR_ERROR;
unsigned long		TR_RECIEVED;
unsigned long		TR_FORWARD;

int main(int argc, char const *argv[]) {
	Router R;
	Graph G;
	pthread_t L[3];
	t_arg *arg = malloc(sizeof(t_arg));

	TR_FORWARD = TR_RECIEVED = TR_ERROR = TR_WARNING = TR_SUCCESS = 0;

	parseArgs(argc, argv);
	printf(BOLD "Carregando configuração do roteador " GREEN "#%d" WHITE "...\n\n" RESET, ROUTER_ID);

	// Carrega para a lista r as configurações dos roteadores lendo
	// os dados do arquivo roteador.config
	R = readRouterConfig();
	if (getRouter(R, ROUTER_ID) == NULL) {
		printf(ERROR "ID definida para o roteador não está configurada!" RESET "\n");
		printf("\nDigite: " BOLD "./Router --help" RESET ", caso precise de ajuda.\n\n");
		exit(1);
	}

	// Carrega para o grafo G as configurações dos enlaces lendo
	// os dados do arquivo enlaces.config
	G = readLinkConfig();
	if (getGraphNode(G, ROUTER_ID) == -1) {
		printf(ERROR "Verifique a configuração de enlaces! Roteador offline." RESET "\n");
		exit(1);
	}

	// Atualiza parent e dist do grafo baseado nas configurações de enlace
	// carregadas para o grafo G.
	GraphSPT(G, getGraphNode(G, ROUTER_ID), G->parent, G->dist);

	LAST_SENT_DGRAM_ID = 0;

	arg->R = R;
	arg->G = G;

	pthread_create(&L[0], NULL, (void *)recieveDatagram, (void *)arg);
	usleep(200000);
	pthread_create(&L[2], NULL, (void *)transmissionControl, NULL);
	usleep(200000);
	pthread_create(&L[1], NULL, (void *)userInterface, (void *)arg);

	// Retorna quando o usuário informar UI_CLOSE(-1) como destino
	pthread_join(L[1], NULL);

	// Força o fechamento das demais threads
	pthread_cancel(L[0]);
	pthread_cancel(L[2]);
	pthread_join(L[0], NULL);
	pthread_join(L[2], NULL);

	// Limpa as estruturas utilizadas
	free(arg);
	destroyGraph(G);
	destroyRouterList(R);
	destroyGlobalQueue();

	// Feito!
	printf(BOLDWHITE "Ok!\n" RESET);
	return 0;
}