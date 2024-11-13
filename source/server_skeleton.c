/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "table.h"
#include "htmessages.pb-c.h"
#include "server_skeleton.h"

/* Inicia o skeleton da tabela. * O main() do servidor deve chamar esta função antes de poder usar a * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna a tabela criada ou NULL em caso de erro. */
struct table_t *server_skeleton_init(int n_lists)
{
    if (n_lists <= 0)
    {
        return NULL;
    }

    struct table_t *table = table_create(n_lists);

    if (table == NULL)
    {
        return NULL;
    }

    // Return the created table
    return table;
}

/* Liberta toda a memória ocupada pela tabela e todos os recursos
 * e outros recursos usados pelo skeleton.
 * Retorna 0 (OK) ou -1 em caso de erro. */
int server_skeleton_destroy(struct table_t *table)
{
    if (table == NULL)
    {
        return -1;
    }
    if (table_destroy(table) != 0)
    {
        return -1;
    }

    return 0;
}

int error(MessageT *msg){
    if (msg == NULL)
    {
        return -1;
    }

    msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
    msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
    return 0;
}


/* Executa na tabela table a operação indicada pelo opcode contido em msg
 * e utiliza a mesma estrutura MessageT para devolver o resultado. * Retorna 0 (OK) ou -1 em caso de erro. */
int invoke(MessageT *msg, struct table_t *table)
{
    if (msg == NULL || table == NULL)
    {
        msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
        msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
        return -1;
    }
    /**
     * MESSAGE_T__OPCODE__OP_BAD = 0,
     * MESSAGE_T__OPCODE__OP_PUT = 10,
     * MESSAGE_T__OPCODE__OP_GET = 20,
     * MESSAGE_T__OPCODE__OP_DEL = 30,
     * MESSAGE_T__OPCODE__OP_SIZE = 40,
     * MESSAGE_T__OPCODE__OP_GETKEYS = 50,
     * MESSAGE_T__OPCODE__OP_GETTABLE = 60,
     * MESSAGE_T__OPCODE__OP_ERROR = 99
     */
    switch (msg->opcode)
    {
    case MESSAGE_T__OPCODE__OP_PUT:
    {
        // PUT
        if (msg->entry == NULL || msg->entry->key == NULL || msg->entry->value.data == NULL)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        if (msg->entry->value.len < 0)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        if (msg->c_type != MESSAGE_T__C_TYPE__CT_ENTRY)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }
        void *value = malloc(msg->entry->value.len);
        if (value == NULL)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }
        memcpy(value, msg->entry->value.data, msg->entry->value.len);

        struct block_t *new_block = block_create(msg->entry->value.len, value);
        if (new_block == NULL)
        {
            free(value);
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }
        if (table_put(table, msg->entry->key, new_block) == -1)
        {
            block_destroy(new_block);
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        block_destroy(new_block);
        msg->opcode = MESSAGE_T__OPCODE__OP_PUT + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
        return 0;
    }
    case MESSAGE_T__OPCODE__OP_GET:
    {
        // GET
        if (msg->key == NULL)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        if (msg->c_type != MESSAGE_T__C_TYPE__CT_KEY)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        struct block_t *block = table_get(table, msg->key);
        if (block == NULL)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        msg->value.len = block->datasize;
        msg->value.data = block->data;
        block_destroy(block);
        msg->opcode = MESSAGE_T__OPCODE__OP_GET + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_VALUE;
        return 0;
    }
    case MESSAGE_T__OPCODE__OP_DEL:
    {
        // DEL
        if (msg->key == NULL)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        if (msg->c_type != MESSAGE_T__C_TYPE__CT_KEY)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        if (table_remove(table, msg->key) == -1)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        msg->opcode = MESSAGE_T__OPCODE__OP_DEL + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
        return 0;
    }
    case MESSAGE_T__OPCODE__OP_SIZE:
    {
        if (msg->c_type != MESSAGE_T__C_TYPE__CT_NONE)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        int size = table_size(table);
        if (size < 0)
        {
            msg->opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg->c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        }

        msg->result = size;
        msg->opcode = MESSAGE_T__OPCODE__OP_SIZE + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_RESULT;
        return 0;
    }
    case MESSAGE_T__OPCODE__OP_GETKEYS:
    {
        if (msg->c_type != MESSAGE_T__C_TYPE__CT_NONE)
        {
            return error(msg);
        }
        char **keys = table_get_keys(table);
        if (keys == NULL)
        {
            return error(msg);
        }

        int n_keys = table_size(table);
        if (n_keys < 0)
        {
            table_free_keys(keys);
            return error(msg);
        }

        msg->n_keys = n_keys;
        msg->keys = keys;
        msg->opcode = MESSAGE_T__OPCODE__OP_GETKEYS + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_KEYS;

        return 0;
    }
    case MESSAGE_T__OPCODE__OP_GETTABLE:
    {
        if (msg->c_type != MESSAGE_T__C_TYPE__CT_NONE)
        {
            return error(msg);
        }

        char **keys = table_get_keys(table);
        if (keys == NULL)
        {
            return error(msg);
        }

        int n_entries = table_size(table);
        if (n_entries < 0)
        {
            table_free_keys(keys);
            return error(msg);
        }

        EntryT **entries = malloc(sizeof(EntryT *) * (n_entries + 1));
        if (entries == NULL)
        {
            
            table_free_keys(keys);
            return error(msg);
        }
        entries[n_entries] = NULL;

        for (int i = 0; i < n_entries; i++)
        {
            struct block_t *data = table_get(table, keys[i]);
            if (data == NULL)
            {
                for (int j = 0; j < i; j++)
                {
                    free(entries[j]);
                }
                free(entries);
                table_free_keys(keys);
                return error(msg);
            }
            EntryT *entry_wrapper = malloc(sizeof(EntryT));
            if (entry_wrapper == NULL)
            {
                printf("erro invoke,get table");
                for (int j = 0; j < i; j++)
                {
                    free(entries[j]);
                }
                free(entries);
                table_free_keys(keys);
                return error(msg);
            }

            entry_wrapper->key = strdup(keys[i]);
            entry_wrapper->value.data = data->data;
            entry_wrapper->value.len = data->datasize;

            entries[i] = entry_wrapper;
            free(data);
        }
        table_free_keys(keys);

        msg->n_entries = n_entries;
        msg->entries = entries;
        msg->opcode = MESSAGE_T__OPCODE__OP_GETTABLE + 1;
        msg->c_type = MESSAGE_T__C_TYPE__CT_TABLE;

        return 0;
    }
    default:
    {
        return error(msg);
    }
    }
}





