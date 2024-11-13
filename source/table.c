/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */
#include "table.h"
#include "table-private.h"
#include "list.h"
#include "list-private.h"
#include "entry.h"
#include "block.h"

#include <stdlib.h>
#include <string.h>

/* Função para calucular um valor de hash
 * key:string.
 * Retorna o valor de hash
 */
unsigned int hash_function(char *key)
{
    unsigned int hash = 0;
    int i = 0;

    while (key[i] != '\0')
    {
        hash += (unsigned int)key[i];
        i++;
    }

    return hash;
}

/* Função para criar e inicializar uma nova tabela hash, com n
 * linhas (n = módulo da função hash).
 * Retorna a tabela ou NULL em caso de erro.
 */
struct table_t *table_create(int n)
{
    if (n <= 0)
    {
        return NULL;
    }
    struct table_t *tb = (struct table_t *)malloc(sizeof(struct table_t));
    if (tb == NULL)
    {
        return NULL;
    }
    tb->size = n;
    tb->lists = (struct list_t **)malloc(n * sizeof(struct list_t *));
    if (tb->lists == NULL)
    {
        free(tb);
        return NULL;
    }
    for (int i = 0; i < n; i++)
    {
        tb->lists[i] = list_create();
        if (tb->lists[i] == NULL)
        {
            while (i > 0)
            {
                list_destroy(tb->lists[--i]);
            }
            free(tb->lists);
            free(tb);
            return NULL;
        }
    }

    return tb;
};

/* Função para adicionar um par chave-valor à tabela. Os dados de entrada
 * desta função deverão ser copiados, ou seja, a função vai criar uma nova
 * entry com *CÓPIAS* da key (string) e dos dados. Se a key já existir na
 * tabela, a função tem de substituir a entry existente na tabela pela
 * nova, fazendo a necessária gestão da memória.
 * Retorna 0 (ok) ou -1 em caso de erro.
 */
int table_put(struct table_t *t, char *key, struct block_t *value)
{
    if (t == NULL || t->lists == NULL || key == NULL || value == NULL)
    {
        return -1;
    }

    int s = t->size;

    int index = hash_function(key) % s;

    struct entry_t *entry = list_get(t->lists[index], key);
    if (entry != NULL)
    {
        block_destroy(entry->value);
        entry->value = block_duplicate(value);
        return 0;
    }
    char *same_key = strdup(key);
    struct block_t *same_value = block_duplicate(value);
    if (same_key == NULL || same_value == NULL)
    {
        if (same_value == NULL)
        {
            free(same_key);
        }
        return -1;
    }

    struct entry_t *new_entry = entry_create(same_key, same_value);
    if (new_entry == NULL)
    {
        free(same_key);
        block_destroy(same_value);
        return -1;
    }

    // add new key entry
    if (list_add(t->lists[index], new_entry) == -1)
    {
        entry_destroy(new_entry);
        return -1;
    }

    return 0;
};

/* Função que procura na tabela uma entry com a chave key.
 * Retorna uma *CÓPIA* dos dados (estrutura block_t) nessa entry ou
 * NULL se não encontrar a entry ou em caso de erro.
 */
struct block_t *table_get(struct table_t *t, char *key)
{
    if (t == NULL || t->lists == NULL || key == NULL)
    {
        return NULL;
    }
    int index = hash_function(key) % t->size;

    // get key entry
    struct entry_t *entry = list_get(t->lists[index], key); // func:list_get
    if (entry == NULL)
    {
        return NULL;
    }

    return block_duplicate(entry->value);
};

/* Função que conta o número de entries na tabela passada como argumento.
 * Retorna o tamanho da tabela ou -1 em caso de erro.
 */
int table_size(struct table_t *t)
{
    if (t == NULL || t->lists == NULL)
    {
        return -1;
    }

    int total_entries = 0;
    for (int i = 0; i < t->size; i++)
    {
        total_entries += list_size(t->lists[i]);
    }

    return total_entries;
};

/* Função auxiliar que constrói um array de char* com a cópia de todas as keys na
 * tabela, colocando o último elemento do array com o valor NULL e
 * reservando toda a memória necessária.
 * Retorna o array de strings ou NULL em caso de erro.
 */
char **table_get_keys(struct table_t *t)
{
    if (t == NULL || t->lists == NULL)
    {
        return NULL;
    }

    int num_keys = 0;
    for (int i = 0; i < t->size; i++)
    {
        num_keys += list_size(t->lists[i]);
    }
    char **keys = (char **)malloc((num_keys + 1) * sizeof(char *));
    if (keys == NULL)
    {
        return NULL;
    }

    int curr_key = 0;
    for (int i = 0; i < t->size; i++)
    {
        char **list_keys = list_get_keys(t->lists[i]);
        if (list_keys == NULL)
        {
            continue;
        }

        for (int j = 0; list_keys[j] != NULL; j++)
        {
            keys[curr_key] = strdup(list_keys[j]);
            if (keys[curr_key] == NULL)
            {
                for (int k = 0; k < curr_key; k++)
                {
                    free(keys[k]);
                }
                free(keys);
                list_free_keys(list_keys);
                return NULL;
            }
            curr_key++;
        }

        list_free_keys(list_keys);
    }

    keys[curr_key] = NULL;

    return keys;
};

/* Função auxiliar que liberta a memória ocupada pelo array de keys obtido pela
 * função table_get_keys.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_free_keys(char **keys)
{
    if (keys == NULL)
    {
        return -1;
    }

    int i = 0;
    while (keys[i] != NULL)
    {
        free(keys[i]);
        i++;
    }

    free(keys);

    return 0;
};

/* Função que remove da lista a entry com a chave key, libertando a
 * memória ocupada pela entry.
 * Retorna 0 se encontrou e removeu a entry, 1 se não encontrou a entry,
 * ou -1 em caso de erro.
 */
int table_remove(struct table_t *t, char *key)
{
    if (t == NULL || t->lists == NULL || key == NULL)
    {
        return -1;
    }

    int s = t->size;

    int index = hash_function(key) % s;

    int after_remove = list_remove(t->lists[index], key);

    return after_remove; // 0:OK,1:key entry not found,-1:erro
};

/* Função que elimina uma tabela, libertando *toda* a memória utilizada
 * pela tabela.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int table_destroy(struct table_t *t)
{
    if (t == NULL || t->lists == NULL)
    {
        return -1;
    }
    // destroy all listas
    for (int i = 0; i < t->size; i++)
    {
        if (t->lists[i] != NULL)
        {
            list_destroy(t->lists[i]);
        }
    }

    free(t->lists);

    free(t);

    return 0;
};