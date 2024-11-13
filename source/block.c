/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */

#include "block.h"
#include <stdlib.h>
#include <string.h>
/* Função que cria um novo bloco de dados block_t e que inicializa
 * os dados de acordo com os argumentos recebidos, sem necessidade de
 * reservar memória para os dados.
 * Retorna a nova estrutura ou NULL em caso de erro.
 */
struct block_t *block_create(int size, void *data)
{
    struct block_t *new_block;
    if (data == NULL || size <= 0)
        return NULL;

    new_block = (struct block_t *)malloc(sizeof(struct block_t));

    if (new_block == NULL)
        return NULL;

    new_block->datasize = size;
    new_block->data = data;

    return new_block;
}

/* Função que duplica uma estrutura block_t, reservando a memória
 * necessária para a nova estrutura.
 * Retorna a nova estrutura ou NULL em caso de erro.
 */
struct block_t *block_duplicate(struct block_t *b)
{
    if (b == NULL || b->datasize <= 0 || b->data == NULL)
        return NULL;
    // duplicar a memoria para o novo block
    void *data_copy = duplicate_memory(b->data, b->datasize);
    if (data_copy == NULL)
        return NULL;

    struct block_t *duplicate = block_create(b->datasize, data_copy);
    if (duplicate == NULL)
    {
        free(data_copy);
        return NULL;
    }

    return duplicate;
}

void *duplicate_memory(void *data_original, int size)
{
    if (data_original == NULL || size <= 0)
        return NULL;

    void *copy = calloc(1, size);

    if (copy == NULL)
        return NULL;

    if (memcpy(copy, data_original, size) == NULL)
    {
        free(copy);
        return NULL;
    }
    return copy;
}

/* Função que substitui o conteúdo de um bloco de dados block_t.
 * Deve assegurar que liberta o espaço ocupado pelo conteúdo antigo.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int block_replace(struct block_t *b, int new_size, void *new_data)
{

    if (b == NULL)
    {
        block_destroy(b);
        return -1;
    }
    b->datasize = new_size;
    strcpy(b->data, new_data);
    return 0;
}

/* Função que elimina um bloco de dados, apontado pelo parâmetro b,
 * libertando toda a memória por ele ocupada.
 * Retorna 0 (OK) ou -1 em caso de erro.
 */
int block_destroy(struct block_t *b)
{
    if (b == NULL)
        return -1;

    free(b->data);
    free(b);
    return 0;
}