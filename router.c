#include <stdio.h>
#include <string.h>
#include "header.h"

Router getRouter(Router r, int ID) {
	Router p;

	if (!r) return NULL;

	for (p = r; p != NULL; p = p->next) {
		if (p->ID == ID)
			return p;
	}

	return NULL;
}

// Calcula por qual roteador a mensagem vai sair
Packet findOutputRoute(Graph G, Router R, Datagram data) {
	int graphNode, destParent;
	Packet out = (Packet)malloc(sizeof(packet));
	memset(out, 0, sizeof(packet));

	Router destRouter;

	if (!G || !R || !data)
		return NULL;

	graphNode = getGraphNode(G, data->destID);
	if (graphNode < 0) {
		printf(ERROR "O destino " BOLD "%d" RESETBOLD" está fora da topologia desta rede." RESET "\n", data->destID);
		return 0;
	}

	destParent = G->ID[G->parent[graphNode]];
	if (destParent == ROUTER_ID)
		destParent = data->destID;

	destRouter = getRouter(R, destParent);
	if (!destRouter) {
		printf(ERROR "Configuração do roteador " BOLD "%d" RESETBOLD" não encontrada. Impossível encontrar uma rota de saída." RESET "\n", data->destID);
		return NULL;
	}

	out->type = TP_NONE;
	out->data = data;
	out->port = destRouter->port;
	out->IP   = (char *)malloc(1 + (sizeof(char) * strlen(destRouter->IP)));
	strcpy(out->IP, destRouter->IP);

	return out;
}
