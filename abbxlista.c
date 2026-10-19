
#include <stdio.h>
#include <stdlib.h>
long comps = 0;
typedef struct No {
    int chave;
    struct No *prox;

} No;

No *lista_insere(No *ini, int k) {
    No *novo = malloc(sizeof(No));
    novo->chave = k;
    novo->prox = ini;
    return novo;
}
No *lista_busca(No *ini, int k) {

    for (No *p = ini; p != NULL; p = p->prox) {
        comps++;
        if (p->chave == k) {
            return p;
        }
    }
    return NULL;
}
void lista_libera(No *ini) {
    while (ini) {
        No *prox = ini->prox;
        free(ini);
        ini = prox;
    }
}

typedef struct Nodo {
    int chave;
    struct Nodo *esq, *dir;

} Nodo;

Nodo *abb_insere(Nodo *raiz, int k) {
    Nodo *novo = malloc(sizeof(Nodo));
    novo->chave = k;
    novo->esq = novo->dir = NULL;
    if (raiz == NULL) {
        return novo;
    }
    Nodo *atual = raiz;
    for (;;) {
        if (k < atual->chave) {
            if (atual->esq == NULL) {
                atual->esq = novo;
                break;  
            }
            atual = atual->esq;
        }
        else {

            if (atual->dir == NULL) {
                atual->dir = novo;
                break;
            }

            atual = atual->dir;
        }
    }
    return raiz;
}
Nodo *abb_busca(Nodo *raiz, int k) {
    Nodo *atual = raiz;
    while (atual != NULL) {
        comps++;
        if (k == atual->chave) {
            return atual;
        }
        atual = (k < atual->chave) ? atual->esq : atual->dir;
    }
    return NULL;
}

void abb_libera(Nodo *raiz) {
    if (raiz == NULL) {
        return;
    }

    abb_libera(raiz->esq);
    abb_libera(raiz->dir);
    free(raiz);
}

long aleat(long n) {

    return (((long)rand() << 15) | rand()) % n;
}

int main(void) {

    int tamanhos[] = {1000, 2000, 3000, 4000, 5000,
                      6000, 7000, 8000, 9000, 10000};

    int n_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);

    int n_sorteios = 1000;
    srand(42);
    FILE *f = fopen("resultados.csv", "w");

    if (!f) {
        perror("resultados.csv");
        return 1;
    }
    fprintf(f, "n,comps_lista,comps_abb\n");
    for (int t = 0; t < n_tamanhos; t++) {
        int n = tamanhos[t];
        int *chaves = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            chaves[i] = i + 1;
        }
        for (int i = n - 1; i > 0; i--) {
            int j = (int)aleat(i + 1);
            int tmp = chaves[i];
            chaves[i] = chaves[j];
            chaves[j] = tmp;
        }

        No *lista = NULL;
        Nodo *abb = NULL;

        for (int i = 0; i < n; i++) {
            lista = lista_insere(lista, chaves[i]);
            abb   = abb_insere(abb, chaves[i]);
        }

        long total_lista = 0;
        long total_abb = 0;

        for (int s = 0; s < n_sorteios; s++) {

            int k = chaves[aleat(n)];
            comps = 0;
            lista_busca(lista, k);
            total_lista += comps; 
            comps = 0;
            abb_busca(abb, k);
            total_abb += comps;
        }
        double media_lista = (double)total_lista / n_sorteios;
        double media_abb   = (double)total_abb   / n_sorteios;
        printf("n = %6d | Lista: %9.1f | ABB: %5.1f\n",
               n, media_lista, media_abb);
        fprintf(f, "%d,%.2f,%.2f\n", n, media_lista, media_abb);
        lista_libera(lista);
        abb_libera(abb);
        free(chaves);
    }

    fclose(f);

    printf("\nResultados gravados em resultados.csv\n");

    return 0;
}
