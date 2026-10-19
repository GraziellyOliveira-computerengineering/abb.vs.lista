/* ====================================================================
 *  LISTA ENCADEADA  vs  ÁRVORE BINÁRIA DE BUSCA (ABB)
 * ====================================================================
 *
 *  O QUE O PROGRAMA FAZ
 *  --------------------
 *  Para cada um de 10 tamanhos de n (1000, 2000, ..., 10000):
 *
 *    1) cria as chaves 1, 2, ..., n e embaralha;
 *    2) insere as chaves, na ordem embaralhada, na lista e na ABB;
 *    3) faz 1000 sorteios: sorteia uma chave e procura nas duas
 *       estruturas, contando quantas comparações cada uma fez;
 *    4) calcula a média das comparações e grava no arquivo
 *       resultados.csv (uma linha por tamanho).
 *
 *  O QUE É UMA "COMPARAÇÃO" AQUI
 *  -----------------------------
 *  Cada vez que a busca olha um nó e compara a chave dele com a
 *  chave procurada, conta 1 comparação.
 *  (Se o seu professor contar "igual" e "menor que" separadamente,
 *  some 2 em vez de 1 nos locais onde aparece comps++.)
 *
 *  RESULTADO ESPERADO (para você conferir se está certo)
 *  ----------------------------------------------------
 *  - Lista: em média n/2 comparações (ex.: n = 10000 -> perto de 5000).
 *    Cresce em linha reta com n.
 *  - ABB:   em média perto de 2*ln(n) comparações (ex.: n = 10000 ->
 *    algo em torno de 17 a 20). Cresce muito devagar.
 *
 *  COMO COMPILAR E RODAR
 *  ---------------------
 *    gcc lista_vs_abb.c -o lista_vs_abb
 *    ./lista_vs_abb            (no Windows: lista_vs_abb.exe)
 * ==================================================================== */


/* --------------------------------------------------------------------
 * BIBLIOTECAS
 * "#include" copia para o seu programa funções prontas de outros
 * arquivos. Os símbolos < > indicam que são bibliotecas do sistema.
 * -------------------------------------------------------------------- */

/* stdio = "standard input/output": printf, fopen, fprintf, fclose */
#include <stdio.h>

/* stdlib = "standard library": malloc, free, rand, srand */
#include <stdlib.h>


/* --------------------------------------------------------------------
 * CONTADOR GLOBAL DE COMPARAÇÕES
 * Está fora de qualquer função para que as duas funções de busca
 * (lista e ABB) consigam somar nele, e a main consiga ler e zerar.
 *
 * "long" é um inteiro maior que "int". Usamos long porque a soma de
 * muitas buscas pode passar de 2 bilhões em tamanhos grandes.
 * -------------------------------------------------------------------- */
long comps = 0;



/* ====================================================================
 *                              LISTA
 * ==================================================================== */

/* --------------------------------------------------------------------
 * "typedef struct" cria um tipo novo, formado por vários campos.
 * Aqui o tipo "No" representa um nó da lista, com dois campos:
 *   - chave: o número guardado;
 *   - prox:  o endereço do próximo nó.
 *
 * O símbolo "*" na declaração significa "ponteiro", ou seja, uma
 * variável que guarda o ENDEREÇO de outra coisa na memória, em vez
 * de guardar o valor em si.
 *
 * "struct No *prox" precisa repetir "struct No" porque o apelido "No"
 * ainda não existe dentro da própria definição.
 * -------------------------------------------------------------------- */
typedef struct No {

    /* valor guardado neste nó */
    int chave;

    /* endereço do próximo nó; no último nó da lista vale NULL (nada) */
    struct No *prox;

} No;


/* --------------------------------------------------------------------
 * lista_insere: coloca uma chave nova no INÍCIO da lista.
 *
 * Por que no início? Porque é rápido (não precisa percorrer a lista
 * toda para achar o fim). A posição de inserção não muda a média de
 * comparações de uma busca, já que as chaves foram embaralhadas.
 *
 * Parâmetros:
 *   ini = endereço do primeiro nó atual (NULL se a lista está vazia)
 *   k   = chave a inserir
 * Retorno: o endereço do novo primeiro nó.
 * -------------------------------------------------------------------- */
No *lista_insere(No *ini, int k) {

    /* malloc reserva memória. sizeof(No) é o tamanho de um nó em bytes.
       "novo" guarda o endereço da memória reservada. */
    No *novo = malloc(sizeof(No));

    /* "->" acessa um campo através de um ponteiro.
       Lê-se: "no nó apontado por novo, campo chave recebe k". */
    novo->chave = k;

    /* o novo nó aponta para o que era o primeiro. Assim ele fica
       "na frente" da lista antiga. */
    novo->prox = ini;

    /* o novo nó é agora o primeiro da lista */
    return novo;
}


/* --------------------------------------------------------------------
 * lista_busca: procura a chave k, nó por nó, a partir do início.
 * Esta é a "busca sequencial": no pior caso olha todos os nós.
 *
 * Retorna o endereço do nó encontrado, ou NULL se não achou.
 * -------------------------------------------------------------------- */
No *lista_busca(No *ini, int k) {

    /* O for tem 3 partes separadas por ";":
         1) No *p = ini    -> começa no primeiro nó
         2) p != NULL      -> continua enquanto p apontar para um nó
                              ("!=" significa "diferente de")
         3) p = p->prox    -> depois de cada volta, anda para o próximo */
    for (No *p = ini; p != NULL; p = p->prox) {

        /* "++" soma 1 à variável. Visitamos um nó, então 1 comparação. */
        comps++;

        /* "==" compara igualdade. (Um só "=" seria atribuição!)
           Se o nó atual tem a chave procurada, achamos. */
        if (p->chave == k) {
            return p;
        }
    }

    /* saiu do for sem achar: percorreu a lista inteira */
    return NULL;
}


/* --------------------------------------------------------------------
 * lista_libera: devolve ao sistema a memória reservada pelos malloc.
 * Em C isso é responsabilidade do programador; sem isso a memória
 * ficaria ocupada até o programa terminar (vazamento de memória).
 * -------------------------------------------------------------------- */
void lista_libera(No *ini) {

    /* Em C, um ponteiro NULL vale "falso" e qualquer outro endereço
       vale "verdadeiro". Então "while (ini)" = "enquanto ini != NULL". */
    while (ini) {

        /* guarda o próximo ANTES de apagar o atual, senão perdemos
           o caminho para o resto da lista */
        No *prox = ini->prox;

        /* apaga o nó atual */
        free(ini);

        /* anda para o próximo nó */
        ini = prox;
    }
}



/* ====================================================================
 *                                ABB
 * ==================================================================== */

/* --------------------------------------------------------------------
 * Nó da árvore: igual ao da lista, mas com DOIS ponteiros de filhos.
 * Regra da ABB:
 *   - chaves MENORES que a do nó ficam na subárvore esquerda;
 *   - chaves MAIORES OU IGUAIS ficam na subárvore direita.
 * Isso permite descartar metade dos nós a cada passo da busca.
 *
 * "*esq, *dir" declara dois ponteiros na mesma linha: cada um precisa
 * do seu próprio "*".
 * -------------------------------------------------------------------- */
typedef struct Nodo {

    /* valor guardado neste nó */
    int chave;

    /* filho da esquerda e filho da direita (NULL se não existir) */
    struct Nodo *esq, *dir;

} Nodo;


/* --------------------------------------------------------------------
 * abb_insere: insere a chave k na árvore cuja raiz é "raiz".
 * Desce pela árvore (esquerda se menor, direita se maior ou igual)
 * até achar um lugar vazio, e pendura o nó novo ali.
 *
 * Retorna a raiz da árvore (que só muda se a árvore estava vazia).
 * -------------------------------------------------------------------- */
Nodo *abb_insere(Nodo *raiz, int k) {

    /* reserva memória para o nó novo e guarda o endereço em "novo" */
    Nodo *novo = malloc(sizeof(Nodo));

    /* preenche o nó novo */
    novo->chave = k;

    /* atribuição encadeada: lê-se da direita para a esquerda.
       Primeiro dir recebe NULL, depois esq recebe esse mesmo valor.
       O nó novo sempre nasce sem filhos. */
    novo->esq = novo->dir = NULL;

    /* árvore vazia: o nó novo é a própria raiz */
    if (raiz == NULL) {
        return novo;
    }

    /* "atual" é o nó que estamos examinando. Começa na raiz. */
    Nodo *atual = raiz;

    /* "for (;;)" é um laço sem condição: repete para sempre, e só
       sai pelo "break" quando a inserção termina. */
    for (;;) {

        /* chave menor que a do nó atual: o lugar dela é à esquerda */
        if (k < atual->chave) {

            /* se o filho esquerdo está livre, penduramos o nó novo */
            if (atual->esq == NULL) {
                atual->esq = novo;
                break;   /* terminou, sai do laço */
            }

            /* senão, descemos para o filho esquerdo e repetimos */
            atual = atual->esq;
        }
        /* chave maior ou igual: o lugar dela é à direita */
        else {

            if (atual->dir == NULL) {
                atual->dir = novo;
                break;
            }

            atual = atual->dir;
        }
    }

    /* a raiz continua a mesma */
    return raiz;
}


/* --------------------------------------------------------------------
 * abb_busca: procura a chave k na árvore.
 * A cada nó, compara e escolhe UM caminho (esquerda ou direita),
 * ignorando a outra metade. Por isso conta tão poucas comparações.
 * -------------------------------------------------------------------- */
Nodo *abb_busca(Nodo *raiz, int k) {

    /* começa a busca pela raiz */
    Nodo *atual = raiz;

    /* continua enquanto "atual" apontar para um nó de verdade */
    while (atual != NULL) {

        /* visitou um nó: 1 comparação */
        comps++;

        /* achou a chave: devolve o nó */
        if (k == atual->chave) {
            return atual;
        }

        /* Operador ternário "condição ? A : B":
             se (k < atual->chave) vale A (vai para a esquerda),
             senão vale B (vai para a direita). */
        atual = (k < atual->chave) ? atual->esq : atual->dir;
    }

    /* "atual" virou NULL: caímos fora da árvore, a chave não existe */
    return NULL;
}


/* --------------------------------------------------------------------
 * abb_libera: devolve a memória de todos os nós da árvore.
 * É RECURSIVA, ou seja, a função chama a si mesma: libera o filho
 * esquerdo, depois o direito, e por último o próprio nó. A ordem
 * importa: se apagássemos o nó primeiro, perderíamos o endereço dos
 * filhos.
 * -------------------------------------------------------------------- */
void abb_libera(Nodo *raiz) {

    /* caso base: árvore (ou subárvore) vazia, não há nada a liberar.
       Sem isso a recursão nunca pararia. */
    if (raiz == NULL) {
        return;
    }

    abb_libera(raiz->esq);
    abb_libera(raiz->dir);
    free(raiz);
}



/* ====================================================================
 *                            UTILITÁRIO
 * ==================================================================== */

/* --------------------------------------------------------------------
 * aleat: sorteia um número inteiro entre 0 e n-1.
 *
 * Por que não usar só rand() % n?
 * Porque, no Windows, rand() só devolve números de 0 a 32767 (15 bits).
 * Com n = 10000 até funcionaria, mas o resultado seria pouco uniforme,
 * e com n maior que 32767 simplesmente não alcançaria todos os valores.
 * Então juntamos duas chamadas para obter um número de 30 bits
 * (até cerca de 1 bilhão).
 *
 * Símbolos usados:
 *   (long)  -> "cast": converte o valor para long antes da conta, para
 *              o deslocamento não estourar o tamanho de um int.
 *   << 15   -> desloca os bits 15 casas para a esquerda, abrindo espaço
 *              para mais 15 bits (equivale a multiplicar por 32768).
 *   |       -> "OU" bit a bit: encaixa a segunda chamada de rand()
 *              nos 15 bits que ficaram livres.
 *   % n     -> resto da divisão por n; força o resultado a ficar
 *              entre 0 e n-1.
 * -------------------------------------------------------------------- */
long aleat(long n) {

    return (((long)rand() << 15) | rand()) % n;
}



/* ====================================================================
 *                               MAIN
 * ==================================================================== */

int main(void) {

    /* ----------------------------------------------------------------
     * PARÂMETROS DO EXPERIMENTO
     * ---------------------------------------------------------------- */

    /* Os 10 tamanhos testados, de mil em mil.
       Por que espaçados igualmente? Para o gráfico mostrar bem que a
       lista cresce em linha reta enquanto a ABB quase não cresce.
       Por que parar em 10000? Na lista, cada busca olha cerca de n/2
       nós. Com n = 10000 e 1000 buscas, são uns 5 milhões de passos
       por tamanho, que o computador faz em frações de segundo. Valores
       bem maiores deixam o programa lento, sem mudar a conclusão. */
    int tamanhos[] = {1000, 2000, 3000, 4000, 5000,
                      6000, 7000, 8000, 9000, 10000};

    /* sizeof(vetor) dá o total de bytes do vetor; dividir pelo tamanho
       de UM elemento dá a quantidade de elementos (aqui, 10).
       Assim, se você mudar a lista de tamanhos, não precisa mexer aqui. */
    int n_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);

    /* Quantas buscas sorteadas por tamanho.
       Uma busca isolada pode dar sorte (achar logo no começo) ou azar.
       Tirando a média de 1000, essas variações se compensam e o
       resultado fica estável. Mais sorteios = média mais precisa. */
    int n_sorteios = 1000;

    /* "Semente" do gerador de números aleatórios. Com semente fixa, o
       programa sorteia SEMPRE a mesma sequência, então seus resultados
       são reproduzíveis (útil para o relatório). O valor 42 é apenas
       uma escolha arbitrária. Para resultados diferentes a cada
       execução, use srand(time(NULL)) e inclua <time.h>. */
    srand(42);


    /* ----------------------------------------------------------------
     * ARQUIVO DE SAÍDA
     * ---------------------------------------------------------------- */

    /* "FILE *" é um ponteiro para um arquivo aberto.
       "w" = write: cria o arquivo (ou apaga o conteúdo se já existe). */
    FILE *f = fopen("resultados.csv", "w");

    /* "!f" significa "f é NULL", isto é, o arquivo não abriu.
       Nesse caso mostramos o motivo do erro e saímos com código 1
       (por convenção, qualquer valor diferente de 0 indica erro). */
    if (!f) {
        perror("resultados.csv");
        return 1;
    }

    /* Primeira linha do CSV: os nomes das colunas.
       "\n" é o caractere de quebra de linha. */
    fprintf(f, "n,comps_lista,comps_abb\n");


    /* ----------------------------------------------------------------
     * LAÇO PRINCIPAL: uma volta para cada tamanho
     * ---------------------------------------------------------------- */
    for (int t = 0; t < n_tamanhos; t++) {

        /* n = tamanho da rodada atual (1000 na 1ª volta, 2000 na 2ª...) */
        int n = tamanhos[t];


        /* ------------------------------------------------------------
         * PASSO 1: criar as chaves 1..n
         * Usamos números únicos e consecutivos para que cada chave
         * exista exatamente uma vez nas estruturas. */

        /* reserva espaço para n inteiros (n * tamanho de um int) */
        int *chaves = malloc(n * sizeof(int));

        /* preenche: chaves[0] = 1, chaves[1] = 2, ..., chaves[n-1] = n.
           Os índices de vetor começam em 0, por isso "i + 1". */
        for (int i = 0; i < n; i++) {
            chaves[i] = i + 1;
        }


        /* ------------------------------------------------------------
         * PASSO 2: embaralhar (algoritmo de Fisher-Yates)
         * Por que embaralhar? Se as chaves entrassem em ordem 1, 2, 3...
         * a ABB viraria uma "escada" para a direita, igual a uma lista,
         * e perderia toda a vantagem. Ordem aleatória deixa a árvore
         * razoavelmente equilibrada.
         *
         * Como funciona: indo de trás para frente, trocamos a posição i
         * com uma posição sorteada entre 0 e i. */
        for (int i = n - 1; i > 0; i--) {

            /* sorteia j entre 0 e i (aleat(i+1) dá de 0 a i).
               "(int)" converte o long devolvido para int. */
            int j = (int)aleat(i + 1);

            /* troca chaves[i] e chaves[j], usando "tmp" para não
               perder um dos valores no meio da troca */
            int tmp = chaves[i];
            chaves[i] = chaves[j];
            chaves[j] = tmp;
        }


        /* ------------------------------------------------------------
         * PASSO 3: inserir as chaves nas duas estruturas
         * As duas recebem as MESMAS chaves na MESMA ordem, para a
         * comparação ser justa. */

        /* começam vazias: NULL = "não aponta para nada" */
        No *lista = NULL;
        Nodo *abb = NULL;

        for (int i = 0; i < n; i++) {

            /* cada insere devolve a nova cabeça/raiz, e nós guardamos
               de volta na mesma variável */
            lista = lista_insere(lista, chaves[i]);
            abb   = abb_insere(abb, chaves[i]);
        }


        /* ------------------------------------------------------------
         * PASSO 4: os sorteios de busca */

        /* acumuladores: somam as comparações de todas as buscas */
        long total_lista = 0;
        long total_abb = 0;

        for (int s = 0; s < n_sorteios; s++) {

            /* sorteia uma POSIÇÃO do vetor (0 a n-1) e pega o valor que
               está lá. Como o vetor tem todas as chaves, a chave
               sorteada sempre existe nas duas estruturas. */
            int k = chaves[aleat(n)];

            /* zera o contador, busca na lista, e soma o que foi contado */
            comps = 0;
            lista_busca(lista, k);
            total_lista += comps;     /* "+=" soma e guarda na variável */

            /* mesma coisa para a ABB, buscando a MESMA chave k */
            comps = 0;
            abb_busca(abb, k);
            total_abb += comps;
        }


        /* ------------------------------------------------------------
         * PASSO 5: calcular as médias
         * "(double)" converte para número com vírgula ANTES de dividir.
         * Sem isso, a divisão de dois inteiros descartaria as casas
         * decimais (ex.: 7 / 2 daria 3 em vez de 3.5). */
        double media_lista = (double)total_lista / n_sorteios;
        double media_abb   = (double)total_abb   / n_sorteios;


        /* ------------------------------------------------------------
         * PASSO 6: mostrar e gravar o resultado
         * Os códigos dentro das aspas são "marcadores de formato":
         *   %d    -> inteiro
         *   %6d   -> inteiro, ocupando pelo menos 6 posições (alinha)
         *   %9.1f -> número com vírgula, 9 posições, 1 casa decimal
         * Cada marcador é trocado, na ordem, pelos valores listados
         * depois da vírgula. */
        printf("n = %6d | Lista: %9.1f | ABB: %5.1f\n",
               n, media_lista, media_abb);

        /* no CSV usamos "%.2f" (2 casas decimais, sem alinhamento) e
           vírgulas como separador entre as colunas */
        fprintf(f, "%d,%.2f,%.2f\n", n, media_lista, media_abb);


        /* ------------------------------------------------------------
         * PASSO 7: limpar a memória desta rodada
         * O próximo n vai criar tudo de novo; sem isso, a memória das
         * rodadas anteriores continuaria ocupada. */
        lista_libera(lista);
        abb_libera(abb);
        free(chaves);
    }


    /* fecha o arquivo; é aqui que o conteúdo termina de ser gravado */
    fclose(f);

    printf("\nResultados gravados em resultados.csv\n");

    /* return 0 = "terminou sem erros" */
    return 0;
}