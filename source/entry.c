/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */
#include "entry.h"
#include <stdlib.h>
#include <string.h>

// struct entry_t {
// 	char *key;	/* string, cadeia de caracteres terminada por '\0' */
// 	struct block_t *value; /* Bloco de dados */
// };

/* Função que cria uma entry, reservando a memória necessária e
 * inicializando-a com a string e o bloco de dados de entrada.
 * Retorna a nova entry ou NULL em caso de erro.
 */
struct entry_t *entry_create(char *key, struct block_t *value)
{
    if (key == NULL || value == NULL)
        return NULL;
    // alocar memoria para a nova entry
    struct entry_t *new_entry = (struct entry_t *)malloc(sizeof(struct entry_t));
    if (new_entry == NULL)
        return NULL;
    // inicializar a nova entry com os valores
    new_entry->key = key;
    new_entry->value = value;

    return new_entry;
}

/* Função que compara duas entries e retorna a ordem das mesmas, sendo esta
 * ordem definida pela ordem das suas chaves.
 * Retorna 0 se as chaves forem iguais, -1 se e1 < e2,
 * 1 se e1 > e2 ou -2 em caso de erro.
 */
int entry_compare(struct entry_t *e1, struct entry_t *e2)
{
    if (e1 == NULL || e2 == NULL)
        return -2;
    if (e1->key == NULL || e2->key == NULL)
        return -2;

    int cmp = strcmp(e1->key, e2->key);
    // strcmp(str1,str2)
    // Se str1=str2, devolve zero;
    // se str1<str2, devolve negativo;
    // se str1>str2, devolve positivo
    if (cmp == 0)
        return 0;
    if (cmp < 0)
        return -1;
    if (cmp > 0)
        return 1;
    return -2;
}

/* Função que duplica uma entry, reservando a memória necessária para a
 * nova estrutura.
 * Retorna a nova entry ou NULL em caso de erro.
 */
struct entry_t *entry_duplicate(struct entry_t *e)
{
    if (e == NULL)
    {
        return NULL;
    }
    // Duplicar a chave
    char *new_key = strdup(e->key);
    if (new_key == NULL)
    {
        return NULL;
    }

    struct block_t *new_value = block_duplicate(e->value);
    if (new_value == NULL)
    {
        free(new_key);
        return NULL;
    }

    struct entry_t *new_entry = entry_create(new_key, new_value);
    if (new_entry == NULL)
    {
        free(new_key);
        block_destroy(new_value);
        return NULL;
    }

    return new_entry;
}

/* Função que substitui o conteúdo de uma entry, usando a nova chave e
 * o novo valor passados como argumentos, e eliminando a memória ocupada
 * pelos conteúdos antigos da mesma.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int entry_replace(struct entry_t *e, char *new_key, struct block_t *new_value)
{
    if (e == NULL || new_key == NULL || new_value == NULL)
    {
        return -1; // caso de erro
    }
    // libertar a memoria, a de antiga
    free(e->key);
    int free_value = block_destroy(e->value);
    if (free_value == 0)
    {
        // definir os novos valores
        e->key = new_key;
        e->value = new_value;
        return 0;
    }

    return -1;
}

/* Função que elimina uma entry, libertando a memória por ela ocupada.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int entry_destroy(struct entry_t *e)
{
    if (e == NULL || e->key == NULL || e->value == NULL)
    {
        return -1; // caso de erro
    }

    // destroy key
    free(e->key);
    // destroy block: value
    block_destroy(e->value);
    free(e);

    return 0;
}