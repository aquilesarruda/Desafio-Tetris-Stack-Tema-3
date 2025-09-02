#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/*
	Gerenciador de Peças – Fila circular (5) e Pilha (3)

	Este programa gerencia duas estruturas: uma fila circular com
	capacidade 5 e uma pilha com capacidade 3. Permite jogar peças,
	reservá-las, recuperá-las e realizar trocas (frente<->topo e 3x3),
	gerando automaticamente novas peças para manter a fila cheia quando
	a fila perde uma peça.
*/

#define QUEUE_CAPACITY 5
#define STACK_CAPACITY 3

typedef struct Peca {
	char nome; /* 'I', 'O', 'T', 'L' */
	int id;    /* ordem de criação */
} Peca;

/* ------------------------- FILA CIRCULAR ------------------------- */
typedef struct Fila {
	Peca itens[QUEUE_CAPACITY];
	int inicio; /* índice do primeiro elemento */
	int tamanho; /* quantidade de elementos na fila */
} Fila;

static void fila_inicializar(Fila *fila) {
	fila->inicio = 0;
	fila->tamanho = 0;
}

static int fila_cheia(const Fila *fila) {
	return fila->tamanho == QUEUE_CAPACITY;
}

static int fila_vazia(const Fila *fila) {
	return fila->tamanho == 0;
}

static int fila_enqueue(Fila *fila, Peca valor) {
	if (fila_cheia(fila)) {
		return 0;
	}
	int fim = (fila->inicio + fila->tamanho) % QUEUE_CAPACITY;
	fila->itens[fim] = valor;
	fila->tamanho++;
	return 1;
}

static int fila_dequeue(Fila *fila, Peca *removida) {
	if (fila_vazia(fila)) {
		return 0;
	}
	if (removida != NULL) {
		*removida = fila->itens[fila->inicio];
	}
	fila->inicio = (fila->inicio + 1) % QUEUE_CAPACITY;
	fila->tamanho--;
	return 1;
}

static int fila_peek_frente(const Fila *fila, Peca *frente) {
	if (fila_vazia(fila)) {
		return 0;
	}
	*frente = fila->itens[fila->inicio];
	return 1;
}

/* Acessa a i-ésima peça a partir da frente (0 é a frente) */
static int fila_get_at(const Fila *fila, int index_from_front, Peca *out) {
	if (index_from_front < 0 || index_from_front >= fila->tamanho) {
		return 0;
	}
	int idx = (fila->inicio + index_from_front) % QUEUE_CAPACITY;
	*out = fila->itens[idx];
	return 1;
}

/* Define a i-ésima peça a partir da frente (0 é a frente) */
static int fila_set_at(Fila *fila, int index_from_front, Peca valor) {
	if (index_from_front < 0 || index_from_front >= fila->tamanho) {
		return 0;
	}
	int idx = (fila->inicio + index_from_front) % QUEUE_CAPACITY;
	fila->itens[idx] = valor;
	return 1;
}

/* ------------------------------ PILHA ----------------------------- */
typedef struct Pilha {
	Peca itens[STACK_CAPACITY];
	int topo; /* quantidade de elementos; topo = tamanho-1 quando >0 */
} Pilha;

static void pilha_inicializar(Pilha *pilha) {
	pilha->topo = 0; /* 0 elementos */
}

static int pilha_cheia(const Pilha *pilha) {
	return pilha->topo == STACK_CAPACITY;
}

static int pilha_vazia(const Pilha *pilha) {
	return pilha->topo == 0;
}

static int pilha_push(Pilha *pilha, Peca valor) {
	if (pilha_cheia(pilha)) {
		return 0;
	}
	pilha->itens[pilha->topo] = valor;
	pilha->topo++;
	return 1;
}

static int pilha_pop(Pilha *pilha, Peca *removida) {
	if (pilha_vazia(pilha)) {
		return 0;
	}
	pilha->topo--;
	if (removida != NULL) {
		*removida = pilha->itens[pilha->topo];
	}
	return 1;
}

static int pilha_peek(const Pilha *pilha, Peca *top) {
	if (pilha_vazia(pilha)) {
		return 0;
	}
	*top = pilha->itens[pilha->topo - 1];
	return 1;
}

/* ------------------------- GERADOR DE PEÇAS ----------------------- */
static int proximoId = 0;

static char gerarTipoAleatorio(void) {
	/* Distribuição uniforme entre I, O, T, L */
	static const char tipos[] = { 'I', 'O', 'T', 'L' };
	int idx = rand() % 4;
	return tipos[idx];
}

static Peca gerarPeca(void) {
	Peca p;
	p.nome = gerarTipoAleatorio();
	p.id = proximoId++;
	return p;
}

/* ---------------------------- EXIBIÇÃO ---------------------------- */
static void exibir_estado(const Fila *fila, const Pilha *pilha) {
	printf("\nEstado atual:\n\n");
	printf("Fila de peças\t");
	for (int i = 0; i < fila->tamanho; ++i) {
		int idx = (fila->inicio + i) % QUEUE_CAPACITY;
		printf("[%c %d] ", fila->itens[idx].nome, fila->itens[idx].id);
	}
	printf("\n");

	printf("Pilha de reserva\t(Topo -> base): ");
	for (int i = pilha->topo - 1; i >= 0; --i) {
		printf("[%c %d] ", pilha->itens[i].nome, pilha->itens[i].id);
	}
	printf("\n");
}

static void exibir_menu(void) {
	printf("\nOpções disponíveis:\n\n");
	printf("Código\tAção\n");
	printf("1\tJogar peça da frente da fila\n");
	printf("2\tEnviar peça da fila para a pilha de reserva\n");
	printf("3\tUsar peça da pilha de reserva\n");
	printf("4\tTrocar peça da frente da fila com o topo da pilha\n");
	printf("5\tTrocar os 3 primeiros da fila com as 3 peças da pilha\n");
	printf("0\tSair\n");
}

/* --------------------------- OPERAÇÕES ---------------------------- */
static void acao_jogar(Fila *fila) {
	Peca p;
	if (!fila_dequeue(fila, &p)) {
		printf("Ação não realizada: fila vazia.\n");
		return;
	}
	printf("Ação: jogou peça da frente da fila: [%c %d].\n", p.nome, p.id);
	/* Gerar nova peça para manter a fila cheia (quando possível) */
	Peca nova = gerarPeca();
	if (!fila_enqueue(fila, nova)) {
		/* Em teoria não deve falhar, mas mantemos robustez */
		printf("Aviso: não foi possível preencher a fila com nova peça.\n");
	}
}

static void acao_reservar(Fila *fila, Pilha *pilha) {
	if (pilha_cheia(pilha)) {
		printf("Ação não realizada: pilha de reserva cheia.\n");
		return;
	}
	Peca p;
	if (!fila_dequeue(fila, &p)) {
		printf("Ação não realizada: fila vazia.\n");
		return;
	}
	if (!pilha_push(pilha, p)) {
		printf("Ação não realizada: erro ao enviar para pilha.\n");
		/* Se falhasse, a peça já foi removida da fila; não retorna ao jogo */
	}
	printf("Ação: peça [%c %d] enviada para a pilha de reserva.\n", p.nome, p.id);
	Peca nova = gerarPeca();
	if (!fila_enqueue(fila, nova)) {
		printf("Aviso: não foi possível preencher a fila com nova peça.\n");
	}
}

static void acao_usar_pilha(Pilha *pilha) {
	Peca p;
	if (!pilha_pop(pilha, &p)) {
		printf("Ação não realizada: pilha vazia.\n");
		return;
	}
	printf("Ação: usou peça reservada do topo da pilha: [%c %d].\n", p.nome, p.id);
}

static void acao_trocar_frente_topo(Fila *fila, Pilha *pilha) {
	if (pilha_vazia(pilha)) {
		printf("Ação não realizada: pilha vazia.\n");
		return;
	}
	if (fila_vazia(fila)) {
		printf("Ação não realizada: fila vazia.\n");
		return;
	}
	int idx_f = fila->inicio;
	int idx_p = pilha->topo - 1;
	Peca tmp = fila->itens[idx_f];
	fila->itens[idx_f] = pilha->itens[idx_p];
	pilha->itens[idx_p] = tmp;
	printf("Ação: troca realizada entre frente da fila e topo da pilha.\n");
}

static void acao_troca_multipla(Fila *fila, Pilha *pilha) {
	if (fila->tamanho < 3 || pilha->topo < 3) {
		printf("Ação não realizada: são necessárias pelo menos 3 peças em cada estrutura.\n");
		return;
	}
	/* Coletar referências aos índices de interesse */
	int f0 = fila->inicio;
	int f1 = (fila->inicio + 1) % QUEUE_CAPACITY;
	int f2 = (fila->inicio + 2) % QUEUE_CAPACITY;
	int p0 = pilha->topo - 1; /* topo */
	int p1 = pilha->topo - 2;
	int p2 = pilha->topo - 3;

	Peca q0 = fila->itens[f0];
	Peca q1 = fila->itens[f1];
	Peca q2 = fila->itens[f2];
	Peca s0 = pilha->itens[p0];
	Peca s1 = pilha->itens[p1];
	Peca s2 = pilha->itens[p2];

	/* Ajuste para combinar com o exemplo: fila recebe s0,s1,s2; 
	   pilha (topo->base) torna-se q2,q1,q0 nos três primeiros níveis */
	fila->itens[f0] = s0;
	fila->itens[f1] = s1;
	fila->itens[f2] = s2;
	
	pilha->itens[p0] = q2;
	pilha->itens[p1] = q1;
	pilha->itens[p2] = q0;

	printf("Ação: troca realizada entre os 3 primeiros da fila e os 3 da pilha.\n");
}

/* ------------------------------ ENTRADA --------------------------- */
static int ler_opcao(void) {
	char buffer[64];
	if (fgets(buffer, (int)sizeof buffer, stdin) == NULL) {
		return 0;
	}
	/* Remover espaços e tentar converter para int */
	char *p = buffer;
	while (isspace((unsigned char)*p)) { ++p; }
	int opcao = 0;
	if (sscanf(p, "%d", &opcao) != 1) {
		return -1; /* inválida */
	}
	return opcao;
}

/* ------------------------------- MAIN ---------------------------- */
int main(void) {
	srand((unsigned int)time(NULL));

	Fila fila;
	Pilha pilha;
	fila_inicializar(&fila);
	pilha_inicializar(&pilha);

	/* Inicializa a fila cheia com 5 peças */
	for (int i = 0; i < QUEUE_CAPACITY; ++i) {
		fila_enqueue(&fila, gerarPeca());
	}

	exibir_estado(&fila, &pilha);

	for (;;) {
		exibir_menu();
		printf("\nOpção escolhida: ");
		int opcao = ler_opcao();
		if (opcao == 0) {
			printf("Encerrando o programa.\n");
			break;
		}
		if (opcao < 0) {
			printf("Opção inválida. Tente novamente.\n");
			exibir_estado(&fila, &pilha);
			continue;
		}

		switch (opcao) {
			case 1:
				acao_jogar(&fila);
				break;
			case 2:
				acao_reservar(&fila, &pilha);
				break;
			case 3:
				acao_usar_pilha(&pilha);
				break;
			case 4:
				acao_trocar_frente_topo(&fila, &pilha);
				break;
			case 5:
				acao_troca_multipla(&fila, &pilha);
				break;
			default:
				printf("Opção inválida. Tente novamente.\n");
		}

		exibir_estado(&fila, &pilha);
	}

	return 0;
}


