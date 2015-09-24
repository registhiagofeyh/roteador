#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"

static Datagram createDatagram(int dest, char *message, Router R);

void sendMessage(int dest, char *message, t_arg *arg) {
	Datagram data;
	Packet p;

	data = createDatagram(dest, message, arg->R);
	if (!data)
		return;

	p = findOutputRoute(arg->G, arg->R, data);
	if (!p)
		return;

	sendDatagram(p, arg);
}

static Datagram createDatagram(int dest, char *message, Router R) {
	Router src  	= getRouter(R, ROUTER_ID);
	Router destR 	= getRouter(R, dest);

	if (!destR || !src) {
		printf("ERRO: Configuração do roteador não encontrada.\n");
		return NULL;
	}

	Datagram data = (Datagram)malloc(sizeof(datagram));

	data->srcID 	= ROUTER_ID;
	data->destID	= dest;
	data->message 	= (char *)malloc(1 + (sizeof(char) * strlen(message)));
	data->destIP 	= (char *)malloc(1 + (sizeof(char) * strlen(destR->IP)));
	data->srcIP 	= (char *)malloc(1 + (sizeof(char) * strlen(src->IP)));
	data->destPort 	= destR->port;
	data->srcPort 	= src->port;
	data->type 		= T_MESSAGE;
	data->code 		= NOT_USED;
	data->TTL 		= DEFAULT_TTL;
	strcpy(data->message, message);
	strcpy(data->destIP, destR->IP);
	strcpy(data->srcIP, src->IP);

	return data;
}

