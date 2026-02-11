#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CAPACIDADE_FILA 5

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

static int filaCheia(const FilaPecas *fila) {
	return fila->tamanho == CAPACIDADE_FILA;
}

static int filaVazia(const FilaPecas *fila) {
	return fila->tamanho == 0;
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

static void exibirMenu(void) {
	printf("\nOpcoes de acao:\n");
	printf("1 - Jogar peca (dequeue)\n");
	printf("2 - Inserir nova peca (enqueue)\n");
	printf("0 - Sair\n");
	printf("Escolha: ");
}

int main(void) {
	FilaPecas fila;
	int opcao = -1;
	int i;

	srand((unsigned int)time(NULL));
	inicializarFila(&fila);

	for (i = 0; i < CAPACIDADE_FILA; i++) {
		enfileirar(&fila, gerarPeca());
	}

	while (opcao != 0) {
		exibirFila(&fila);
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
			} else {
				printf("Fila vazia. Nao ha peca para jogar.\n");
			}
		} else if (opcao == 2) {
			if (enfileirar(&fila, gerarPeca())) {
				printf("Nova peca inserida.\n");
			} else {
				printf("Fila cheia. Nao e possivel inserir.\n");
			}
		} else if (opcao == 0) {
			printf("Saindo...\n");
		} else {
			printf("Opcao invalida.\n");
		}
	}

	return 0;
}
