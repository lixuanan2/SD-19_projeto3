/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */
#include "list.h"
#include "list-private.h"
#include <stdlib.h>
#include "entry.h"
#include <string.h>

/* Função que cria e inicializa uma nova lista (estrutura list_t a
 * ser definida pelo grupo no ficheiro list-private.h).
 * Retorna a lista ou NULL em caso de erro.
 */
struct list_t *list_create()
{
    struct list_t *lista = (struct list_t *)malloc(sizeof(struct list_t));

    if (lista == NULL)
    {
        return NULL; // caso de erro
    }

    // inicializar
    lista->head = NULL;
    lista->size = 0;

    return lista;
}

/* Função que adiciona à lista a entry passada como argumento.
 * A entry é inserida de forma ordenada, tendo por base a comparação
 * de entries feita pela função entry_compare do módulo entry e
 * considerando que a entry menor deve ficar na cabeça da lista.
 * Se já existir uma entry igual (com a mesma chave), a entry
 * já existente na lista será substituída pela nova entry,
 * sendo libertada a memória ocupada pela entry antiga.
 * Retorna 0 se a entry ainda não existia, 1 se já existia e foi
 * substituída, ou -1 em caso de erro.
 */
int list_add(struct list_t *l, struct entry_t *entry)
{

    if (l == NULL || entry == NULL)
    {
        return -1;
    }

    struct node_t *new = (struct node_t *)malloc(sizeof(struct node_t));
    if (new == NULL)
    {
        return -1;
    }

    struct node_t *cur = l->head;
    struct node_t *prev = NULL;

    // se vazio
    if (l->head == NULL)
    {
        l->head = new;
        new->entry = entry;
        new->next = NULL;
        l->size++;
        return 0;
    }

    // iteração
    while (cur != NULL && entry_compare(cur->entry, entry) < 0)
    {
        prev = cur;
        cur = cur->next;
    }

    // se encontra a mesma key
    if (cur != NULL && entry_compare(cur->entry, entry) == 0)
    {
        // substituir a entry
        struct entry_t *old_entry = cur->entry;
        cur->entry = entry;
        entry_destroy(old_entry);
        free(new);
        return 1;
    }

    // novo nó
    new->entry = entry;
    new->next = cur;
    if (prev == NULL)
    {
        l->head = new;
    }
    else
    {
        prev->next = new;
    }
    l->size++;
    return 0;
}

/* Função que conta o número de entries na lista passada como argumento.
 * Retorna o tamanho da lista ou -1 em caso de erro.
 */
int list_size(struct list_t *l)
{
    if (l == NULL)
    {
        return -1;
    }
    return l->size;
}

/* Função que obtém da lista a entry com a chave key.
 * Retorna a referência da entry na lista ou NULL se não encontrar a
 * entry ou em caso de erro.
 */
struct entry_t *list_get(struct list_t *l, char *key)
{
    if (l == NULL || key == NULL)
    {
        return NULL;
    }

    struct node_t *current = l->head;

    while (current != NULL)
    {
        if (strcmp(current->entry->key, key) == 0)
        {
            return current->entry;
        }
        current = current->next;
    }
    return NULL;
}

/* Função auxiliar que constrói um array de char* com a cópia de todas as keys na
 * lista, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
char **list_get_keys(struct list_t *l)
{
    if (l == NULL || l->head == NULL)
    {
        return NULL;
    }

    // reservar
    char **keys = malloc((l->size + 1) * sizeof(char *));
    if (keys == NULL)
    {
        return NULL; // caso de erro
    }

    struct node_t *current = l->head;
    int i = 0;

    while (current != NULL)
    {
        // duplicar a string
        keys[i] = strdup(current->entry->key);
        if (keys[i] == NULL)
        {
            // se nao reservou com sucesso
            while (i > 0)
            {
                free(keys[--i]);
            }
            free(keys);
            return NULL;
        }
        current = current->next;
        i++;
    }
    
    keys[i] = NULL; 
    return keys;
}

/* Função auxiliar que liberta a memória ocupada pelo array de keys obtido pela
 * função list_get_keys.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int list_free_keys(char **keys)
{
    if (keys == NULL)
    {
        return -1; // caso de erro
    }

    int i = 0;
    while (keys[i] != NULL)
    {
        free(keys[i]);
        i++;
    }

    free(keys);
    return 0;
}

/* Função que elimina da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int list_remove(struct list_t *l, char *key)
{
    if (l == NULL || key == NULL || l->head == NULL)
    {
        return -1; // caso de erro
    }

    struct node_t *current = l->head;
    struct node_t *prev = NULL;

    while (current != NULL && strcmp(current->entry->key, key) != 0)
    { // encontra node
        prev = current;
        current = current->next;
    }

    if (current == NULL)
    {
        return 1; // não encontrou a entry
    }

    if (prev == NULL)
    {
        l->head = current->next;
    }
    else
    {
        prev->next = current->next;
    }

    entry_destroy(current->entry);
    free(current);
    l->size--;

    return 0;
}

/* Função que elimina uma lista, libertando *toda* a memória utilizada
 * pela lista (incluindo todas as suas entradas).
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int list_destroy(struct list_t *l)
{
    struct node_t *current = l->head;
    while (current != NULL)
    {
        struct node_t *next = current->next; // guarda a proxima node
        entry_destroy(current->entry);       // free o entry_t atual
        free(current);                       // free node atual
        current = next;
    }
    free(l);
    return 0;
}
