#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define CAPACIDADE_FILA 5
#define CAPACIDADE_PILHA 3

typedef struct {
	char nome;
	int id;
} Peca;

typedef struct {
	Peca itens[CAPACIDADE_FILA];
	int frente;
	int tras;
	int tamanho;
} FilaPecas;

typedef struct {
	Peca itens[CAPACIDADE_PILHA];
	int topo;
} PilhaPecas;

typedef enum {
	ACAO_INVALIDA = 0,
	ACAO_JOGAR,
	ACAO_RESERVAR,
	ACAO_USAR_RESERVADA,
	ACAO_TROCAR,
	ACAO_INVERTER
} TipoAcao;

typedef struct {
	FilaPecas fila;
	PilhaPecas pilha;
	TipoAcao acao;
	bool ativo;
} HistoricoAcao;

static Peca gerarPeca(void) {
	static int proximoId = 0;
	const char tipos[] = { 'I', 'O', 'T', 'L' };
	Peca nova;

	nova.nome = tipos[rand() % (int)(sizeof(tipos) / sizeof(tipos[0]))];
	nova.id = proximoId++;
	return nova;
}

static void salvarHistorico(HistoricoAcao *registro, const FilaPecas *fila, const PilhaPecas *pilha, TipoAcao acao) {
	registro->fila = *fila;
	registro->pilha = *pilha;
	registro->acao = acao;
	registro->ativo = true;
}

static int desfazerHistorico(HistoricoAcao *registro, FilaPecas *fila, PilhaPecas *pilha) {
	if (!registro->ativo) {
		return 0;
	}
	*fila = registro->fila;
	*pilha = registro->pilha;
	registro->ativo = false;
	return 1;
}

static void inicializarFila(FilaPecas *fila) {
	fila->frente = 0;
	fila->tras = 0;
	fila->tamanho = 0;
}

static void inicializarPilha(PilhaPecas *pilha) {
	pilha->topo = 0;
}

static int filaCheia(const FilaPecas *fila) {
	return fila->tamanho == CAPACIDADE_FILA;
}

static int filaVazia(const FilaPecas *fila) {
	return fila->tamanho == 0;
}

static int pilhaCheia(const PilhaPecas *pilha) {
	return pilha->topo == CAPACIDADE_PILHA;
}

static int pilhaVazia(const PilhaPecas *pilha) {
	return pilha->topo == 0;
}

static int enfileirar(FilaPecas *fila, Peca peca) {
	if (filaCheia(fila)) {
		return 0;
	}

	fila->itens[fila->tras] = peca;
	fila->tras = (fila->tras + 1) % CAPACIDADE_FILA;
	fila->tamanho++;
	return 1;
}

static int desenfileirar(FilaPecas *fila, Peca *removida) {
	if (filaVazia(fila)) {
		return 0;
	}

	*removida = fila->itens[fila->frente];
	fila->frente = (fila->frente + 1) % CAPACIDADE_FILA;
	fila->tamanho--;
	return 1;
}

static int empilhar(PilhaPecas *pilha, Peca peca) {
	if (pilhaCheia(pilha)) {
		return 0;
	}

	pilha->itens[pilha->topo] = peca;
	pilha->topo++;
	return 1;
}

static int desempilhar(PilhaPecas *pilha, Peca *removida) {
	if (pilhaVazia(pilha)) {
		return 0;
	}

	pilha->topo--;
	*removida = pilha->itens[pilha->topo];
	return 1;
}

static void exibirFila(const FilaPecas *fila) {
	int i;

	printf("\nFila de pecas\n");
	if (filaVazia(fila)) {
		printf("(fila vazia)\n");
		return;
	}

	for (i = 0; i < fila->tamanho; i++) {
		int indice = (fila->frente + i) % CAPACIDADE_FILA;
		printf("[%c %d] ", fila->itens[indice].nome, fila->itens[indice].id);
	}
	printf("\n");
}

static void exibirPilha(const PilhaPecas *pilha) {
	int i;

	printf("\nPilha de reserva\n");
	if (pilhaVazia(pilha)) {
		printf("(pilha vazia)\n");
		return;
	}

	for (i = pilha->topo - 1; i >= 0; i--) {
		printf("[%c %d] ", pilha->itens[i].nome, pilha->itens[i].id);
	}
	printf("\n");
}

static int jogarPeca(FilaPecas *fila, PilhaPecas *pilha, HistoricoAcao *historico) {
	if (filaVazia(fila)) {
		printf("Fila vazia. Nao ha peca para jogar.\n");
		return 0;
	}
	salvarHistorico(historico, fila, pilha, ACAO_JOGAR);
	Peca removida;
	desenfileirar(fila, &removida);
	printf("Peca jogada: [%c %d]\n", removida.nome, removida.id);
	enfileirar(fila, gerarPeca());
	return 1;
}

static int reservarPeca(FilaPecas *fila, PilhaPecas *pilha, HistoricoAcao *historico) {
	if (pilhaCheia(pilha)) {
		printf("Pilha cheia. Nao e possivel reservar.\n");
		return 0;
	}
	if (filaVazia(fila)) {
		printf("Fila vazia. Nao ha peca para reservar.\n");
		return 0;
	}
	salvarHistorico(historico, fila, pilha, ACAO_RESERVAR);
	Peca removida;
	desenfileirar(fila, &removida);
	empilhar(pilha, removida);
	printf("Peca reservada: [%c %d]\n", removida.nome, removida.id);
	enfileirar(fila, gerarPeca());
	return 1;
}

static int usarPecaReservada(FilaPecas *fila, PilhaPecas *pilha, HistoricoAcao *historico) {
	if (pilhaVazia(pilha)) {
		printf("Pilha vazia. Nao ha peca reservada.\n");
		return 0;
	}
	salvarHistorico(historico, fila, pilha, ACAO_USAR_RESERVADA);
	Peca removida;
	desempilhar(pilha, &removida);
	printf("Peca reservada usada: [%c %d]\n", removida.nome, removida.id);
	return 1;
}

static int trocarTopoComFrente(FilaPecas *fila, PilhaPecas *pilha, HistoricoAcao *historico) {
	if (filaVazia(fila)) {
		printf("Fila vazia. Nada para trocar.\n");
		return 0;
	}
	if (pilhaVazia(pilha)) {
		printf("Pilha vazia. Nada para trocar.\n");
		return 0;
	}
	salvarHistorico(historico, fila, pilha, ACAO_TROCAR);
	int indiceFila = fila->frente;
	int indicePilha = pilha->topo - 1;
	Peca temp = fila->itens[indiceFila];
	fila->itens[indiceFila] = pilha->itens[indicePilha];
	pilha->itens[indicePilha] = temp;
	printf("Topo da pilha trocado com frente da fila.\n");
	return 1;
}

static int inverterFilaComPilha(FilaPecas *fila, PilhaPecas *pilha, HistoricoAcao *historico) {
	int quantidade = fila->tamanho < pilha->topo ? fila->tamanho : pilha->topo;
	if (quantidade == 0) {
		printf("Nao ha pares para inverter. Garanta pecas na fila e na pilha.\n");
		return 0;
	}
	salvarHistorico(historico, fila, pilha, ACAO_INVERTER);
	for (int i = 0; i < quantidade; i++) {
		int indiceFila = (fila->frente + i) % CAPACIDADE_FILA;
		int indicePilha = pilha->topo - 1 - i;
		Peca temp = fila->itens[indiceFila];
		fila->itens[indiceFila] = pilha->itens[indicePilha];
		pilha->itens[indicePilha] = temp;
	}
	printf("%d par(es) de pecas foram invertidos entre fila e pilha.\n", quantidade);
	return 1;
}

static void exibirMenu(void) {
	printf("\nOpcoes de acao:\n");
	printf("1 - Jogar peca\n");
	printf("2 - Reservar peca\n");
	printf("3 - Usar peca reservada\n");
	printf("4 - Trocar topo da pilha com frente da fila\n");
	printf("5 - Desfazer ultima jogada\n");
	printf("6 - Inverter fila com pilha\n");
	printf("0 - Sair\n");
	printf("Escolha: ");
}

int main(void) {
	FilaPecas fila;
	PilhaPecas pilha;
	HistoricoAcao historico = {0};
	int opcao = -1;
	int i;

	srand((unsigned int)time(NULL));
	inicializarFila(&fila);
	inicializarPilha(&pilha);

	for (i = 0; i < CAPACIDADE_FILA; i++) {
		enfileirar(&fila, gerarPeca());
	}

	exibirFila(&fila);
	exibirPilha(&pilha);

	while (opcao != 0) {
		exibirMenu();

		if (scanf("%d", &opcao) != 1) {
			int ch;
			printf("Entrada invalida.\n");
			while ((ch = getchar()) != '\n' && ch != EOF) {
			}
			opcao = -1;
			continue;
		}

		switch (opcao) {
			case 1:
				jogarPeca(&fila, &pilha, &historico);
				break;
			case 2:
				reservarPeca(&fila, &pilha, &historico);
				break;
			case 3:
				usarPecaReservada(&fila, &pilha, &historico);
				break;
			case 4:
				trocarTopoComFrente(&fila, &pilha, &historico);
				break;
			case 5:
				if (desfazerHistorico(&historico, &fila, &pilha)) {
					printf("Ultima jogada desfeita.\n");
				} else {
					printf("Nao ha acao para desfazer.\n");
				}
				break;
			case 6:
				inverterFilaComPilha(&fila, &pilha, &historico);
				break;
			case 0:
				printf("Saindo...\n");
				break;
			default:
				printf("Opcao invalida.\n");
				break;
		}

		if (opcao != 0) {
			exibirFila(&fila);
			exibirPilha(&pilha);
		}
	}

	return 0;
}
