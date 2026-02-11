#include <stdio.h>
#include <stdlib.h>
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

static Peca gerarPeca(void) {
	static int proximoId = 0;
	const char tipos[] = { 'I', 'O', 'T', 'L' };
	Peca nova;

	nova.nome = tipos[rand() % (int)(sizeof(tipos) / sizeof(tipos[0]))];
	nova.id = proximoId++;
	return nova;
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

static void exibirMenu(void) {
	printf("\nOpcoes de acao:\n");
	printf("1 - Jogar peca\n");
	printf("2 - Reservar peca\n");
	printf("3 - Usar peca reservada\n");
	printf("0 - Sair\n");
	printf("Escolha: ");
}

int main(void) {
	FilaPecas fila;
	PilhaPecas pilha;
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

		if (opcao == 1) {
			Peca removida;
			if (desenfileirar(&fila, &removida)) {
				printf("Peca jogada: [%c %d]\n", removida.nome, removida.id);
				enfileirar(&fila, gerarPeca());
			} else {
				printf("Fila vazia. Nao ha peca para jogar.\n");
			}
		} else if (opcao == 2) {
			Peca removida;
			if (pilhaCheia(&pilha)) {
				printf("Pilha cheia. Nao e possivel reservar.\n");
			} else if (desenfileirar(&fila, &removida)) {
				empilhar(&pilha, removida);
				printf("Peca reservada: [%c %d]\n", removida.nome, removida.id);
				enfileirar(&fila, gerarPeca());
			} else {
				printf("Fila vazia. Nao ha peca para reservar.\n");
			}
		} else if (opcao == 3) {
			Peca removida;
			if (desempilhar(&pilha, &removida)) {
				printf("Peca reservada usada: [%c %d]\n", removida.nome, removida.id);
			} else {
				printf("Pilha vazia. Nao ha peca reservada.\n");
			}
		} else if (opcao == 0) {
			printf("Saindo...\n");
		} else {
			printf("Opcao invalida.\n");
		}

		if (opcao != 0) {
			exibirFila(&fila);
			exibirPilha(&pilha);
		}
	}

	return 0;
}
