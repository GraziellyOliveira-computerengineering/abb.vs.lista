// Bibliotecas que vou usar e códigos de leitura
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define LEITURA_OK 1
#define CLAUSULA_VAZIA 0
#define ERRO_LEITURA -1

// -1: false, 1: true, 0: indefinido(depende de outro literal)
#define TRUE 1
#define INDEFINIDO 0
#define FALSE -1

// -------------------------------------------------------------------------------------------------------------------------------------------------------------//
// Structs

typedef struct node
{
    int literal;
    struct node* next;
} node;

typedef struct
{
    node* head;
    node* tail;
} clausula;

typedef struct tree
{
    int literal;
    bool valor;
    int interpretacao; // -1: false, 1: true, 0: indefinido(depende de outro literal)
    struct tree* filho_direita;
    struct tree* filho_esquerda;
}tree;


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//

// Funções que vou usar

void adicionar_na_clausula(clausula* c, int literal);
int avaliar_clausula(clausula* c, int* atribuicao);
int avaliar_formula(clausula* c, int qnt_clausulas, int* atribuicao);
tree* criar_subtree(int literal, bool valor);
void inicializar_clausula(clausula* c, int qnt_clausulas);
void inicializar_tree(tree* t);
int ler_cabecalho(int* qnt_literais, int* qnt_clausulas);
int ler_clausulas(clausula* c, int qnt_clausulas);
void liberar_clausula(clausula* c);
void liberar_clausulas(clausula *c, int qnt_clausulas);
void liberar_tree(tree *t);
int proximo_literal_livre(int* atribuicao, int qnt_literais);
int sat(tree* t, clausula* c, int qnt_clausulas, int* atribuicao, int qnt_literais);
int valor_literal(int literal, int* atribuicao);


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


int main(void)
{
    int qnt_literais, qnt_clausulas;
    scanf("%d %d", &qnt_literais, &qnt_clausulas);

    // Vendo se tem clausulas ou não, se não tiver, é sat automaticamente
    
    if (ler_cabecalho(&qnt_literais, &qnt_clausulas) == ERRO_LEITURA)
    {
        printf("Erro na leitura do arquivo!\n");
        return 1;
    }
    
    if (qnt_clausulas == 0)
    {
        printf("\nSAT!\n");
        return 0;
    }
    // Uso calloc para todas as posições começarem como indefinidas(0)
    // Array reponsável por guardar os valores das variáveis para acesso rápido
    int* atribuicao = (int*) calloc(qnt_literais + 1, sizeof(int)); // Uso o +1 para a variável n ficar no índice n (a posição 0 não é usada)

    if (atribuicao == NULL) // Testando se tem memória suficiente
    {
        printf("Erro de memoria!\n");
        return 1;
    }   

    // Lendo as clausulas e checando se a leitura deu certo.
    clausula c[qnt_clausulas];
    inicializar_clausula(c, qnt_clausulas);

    int status = ler_clausulas(c, qnt_clausulas);

    if (status == ERRO_LEITURA)
    {
        printf("Erro na leitura do arquivo!\n");
        liberar_clausulas(c, qnt_clausulas);
        free(atribuicao);
        return 1;
    }

    if (status == CLAUSULA_VAZIA)
    {
        printf("UNSAT!\n");
        liberar_clausulas(c, qnt_clausulas);
        free(atribuicao);
        return 0;
    }


    // Criando a árvore para começar a busca
    tree *t = criar_subtree(0, true);

    int decisao = sat(t, c, qnt_clausulas, atribuicao, qnt_literais);

    if (decisao == TRUE)
    {
        printf("\nSAT!\n");
        for (int i = 1; i <= qnt_literais; i++)
        {
            if (atribuicao[i] == TRUE)
            {
                printf("%d = 1\n", i);
            }
            else
            {
                printf("%d = 0\n", i);
            }
        }
    }
    else
    {
        printf("UNSAT!\n");
    }

    liberar_tree(t);
    liberar_clausulas(c, qnt_clausulas);
    free(atribuicao);
    return 0;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para adicionar um literal na clausula
void adicionar_na_clausula(clausula* c, int literal)
{
    node* aux = (node*) malloc(sizeof(node));
    if (aux == NULL) // Checando se ainda tem memória disponível para não quebrar o programa
    {
        return;
    }
    aux->literal = literal;
    aux->next = NULL;
    
    if (c->head == NULL)
    {
        c->head = aux;
        c->tail = aux;
        return;
    }

    c->tail->next = aux;
    c->tail = aux;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para interpretação parcial da clausula. Como a clausula são OR, se achar um true retorna true, se não achar nenhum, se tiver uma variável ainda retorna indefinido, se não, retorna false
int avaliar_clausula(clausula* c, int* atribuicao)
{
    node* aux = c->head;
    bool flag = false;
    while (aux != NULL)
    {
        int valor = valor_literal(aux->literal, atribuicao);
        if (valor == TRUE)
        {
            return TRUE;
        }

        if (valor == INDEFINIDO)
        {
            flag = true;
        }

        aux = aux->next;
    }

    if (flag)
    {
        return INDEFINIDO;
    }
    else
    {
        return FALSE;
    }
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para avaliar a fórmula(fazer a interpretação parcial das cláusulas). Como são AND, se achar um false retorna false(Vai para a atribuição de outra variável). Se não, procura por um indefinido. Se não tiver, retorna true (SAT)
int avaliar_formula(clausula* c, int qnt_clausulas, int* atribuicao)
{
    bool flag = false;
    for (int i = 0; i < qnt_clausulas; i++)
    {
        int avaliacao = avaliar_clausula(&c[i], atribuicao);
        if (avaliacao == FALSE)
        {
            return FALSE;
        }

        if (avaliacao == INDEFINIDO)
        {
            flag = true;
        }
    }

    if (flag)
    {
        return INDEFINIDO;
    }
    else
    {
        return TRUE;
    }
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para criar um novo filho da árvore e colocar os valores
tree* criar_subtree(int literal, bool valor)
{
    tree* aux = (tree*)malloc(sizeof(tree));
    if (aux == NULL) // Checando se ainda tem memória disponível para não quebrar o programa
    {
        return NULL;
    }
    inicializar_tree(aux);
    aux->literal = literal;
    aux->valor = valor;

    return aux;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para iniciar clausula
void inicializar_clausula(clausula* c, int qnt_clausulas)
{
    for (int i = 0; i < qnt_clausulas; i++)
    {
        c[i].head = NULL;
        c[i].tail = NULL;
    }
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para inicializar a arvore
void inicializar_tree(tree* t)
{
    t->filho_direita = NULL;
    t->filho_esquerda = NULL;
    t->interpretacao = INDEFINIDO;
    t->literal = 0; // O literal da raiz é 0 porque a raiz não importa para a interpretação
    t->valor = true;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para ler a quantidade de literais e de clausulas, ignorando as linhas de comentário
int ler_cabecalho(int* qnt_literais, int* qnt_clausulas)
{
    char linha[256];

    while (fgets(linha, sizeof(linha), stdin) != NULL)
    {
        if (linha[0] == 'c')
        {
            continue;
        }

        // sscanf lê de uma string ao invés do teclado/arquivo (a linha com o p cnf n1 n2 já foi lida e está salva em "linha")
        if (sscanf(linha, "p cnf %d %d", qnt_literais, qnt_clausulas) == 2)
        {
            return LEITURA_OK;
        }
    }

    return ERRO_LEITURA;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para ler as clausulas e se tiver uma vazia já retornar UNSAT (Se tiver vazia vai ficar expressão1 ^ expressão2 ^ 0 que dá falso sempre)
int ler_clausulas(clausula* c, int qnt_clausulas)
{
    for (int i = 0; i < qnt_clausulas; i++)
    {
        int flag = 1;
        while (1)
        {
            int aux;
            if (scanf("%d", &aux) != 1)
            {
                return ERRO_LEITURA;
            }
            if (aux == 0 && flag)
            {
                return CLAUSULA_VAZIA;
            }
            
            if (aux == 0)
            {
                break;
            }
            adicionar_na_clausula(&c[i], aux);
            flag = 0;
        }
    }

    return LEITURA_OK;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para liberar uma clausula
void liberar_clausula(clausula* c)
{
    node *atual = c->head;

    while(atual != NULL)
    {
        node *proximo = atual->next;
        free(atual);
        atual = proximo;
    }
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para lberar o array de clausulas
void liberar_clausulas(clausula *c, int qnt_clausulas)
{
    for (int i = 0; i < qnt_clausulas; i++)
    {
        liberar_clausula(&c[i]);
    }
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função  para liberar memória da tree
void liberar_tree(tree *t)
{
    if (t == NULL)
    {
        return;
    }

    liberar_tree(t->filho_direita);
    liberar_tree(t->filho_esquerda);

    free(t);
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//

// Função para procurar qual o próximo literal livre
int proximo_literal_livre(int* atribuicao, int qnt_literais)
{
    for (int i = 1; i <= qnt_literais; i++)
    {
        if (atribuicao[i] == INDEFINIDO)
        {
            return i;
        }
    }

    return 0;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função do slide. Função para buscar uma combinação de variáveis da árvore que façam ser sat
int sat(tree* t, clausula* c, int qnt_clausulas, int* atribuicao, int qnt_literais)
{
    int formula = avaliar_formula(c, qnt_clausulas, atribuicao);
    t->interpretacao = formula;

    if (formula == TRUE)
    {
        return TRUE;
    }

    if (formula == FALSE)
    {
        return FALSE;
    }

    int literal_livre = proximo_literal_livre(atribuicao, qnt_literais);

    atribuicao[literal_livre] = TRUE;
    t->filho_esquerda = criar_subtree(literal_livre, true);
    int interpretacao_parcial = sat(t->filho_esquerda, c, qnt_clausulas, atribuicao, qnt_literais);
    
    if (interpretacao_parcial == TRUE)
    {
        return TRUE;
    }

    atribuicao[literal_livre] = FALSE;
    t->filho_direita = criar_subtree(literal_livre, false);
    interpretacao_parcial = sat(t->filho_direita, c, qnt_clausulas, atribuicao, qnt_literais);

    if (interpretacao_parcial == TRUE)
    {
        return TRUE;
    }

    atribuicao[literal_livre] = INDEFINIDO;
    return FALSE;
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------------//


// Função para fazer a negação de um literal, se necessário
int valor_literal(int literal, int* atribuicao)
{
    if (literal < 0)
    {
        return -atribuicao[-literal];
    }

    return atribuicao[literal];
}