/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */

#include "block.h"
#include "entry.h"
#include "client_network.h"
#include "client_stub.h"
#include "client_stub-private.h"
#include "message.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "htmessages.pb-c.h"
#include <arpa/inet.h>

/* Função para estabelecer uma associação entre o cliente e o servidor,
 * em que address_port é uma string no formato <hostname>:<port>.
 * Retorna a estrutura rtable preenchida, ou NULL em caso de erro.
 */
struct rtable_t *rtable_connect(char *address_port)
{
    if (address_port == NULL)
    {
        return NULL;
    }

    char *address_copy = strdup(address_port);
    if (address_copy == NULL) {
        perror("Error duplicating address_port");
        return NULL;
    }

    char *ip_str = strtok(address_port, ":");
    char *port_str = strtok(NULL, ":");

    if (ip_str == NULL || port_str == NULL)
    {
        fprintf(stderr, "Error: IP or port is missing in address_port\n");
        free(address_copy);
        return NULL;
    }

    struct rtable_t *table = malloc(sizeof(struct rtable_t));
    if (!table)
    {
        perror("Error allocating memory for rtable_t");
        free(address_copy);
        return NULL;
    }

    table->server_address = strdup(ip_str);
    table->server_port = atoi(port_str);
    table->sockfd = -1; // Initialiaze

    free(address_copy);

    if (network_connect(table) == -1)
    {
        fprintf(stderr, "Error: Failed to connect to server\n");
        free(table->server_address);
        free(table);
        return NULL;
    }

    return table;
}

/* Termina a associação entre o cliente e o servidor, fechando a
 * ligação com o servidor e libertando toda a memória local. * Retorna 0 se tudo correr bem, ou -1 em caso de erro. */
int rtable_disconnect(struct rtable_t *rtable)
{
    if (rtable == NULL)
    {
        return -1;
    }

    if (network_close(rtable) == -1)
    {
        return -1;
    }
    free(rtable->server_address);
    if (rtable->sockfd != -1)
    {
        close(rtable->sockfd);
    }
    free(rtable);

    return 0;
}

/* Função para adicionar uma entrada na tabela.
 * Se a key já existe, vai substituir essa entrada pelos novos dados.
 * Retorna 0 (OK, em adição/substituição), ou -1 (erro).
 */
int rtable_put(struct rtable_t *rtable, struct entry_t *entry)
{
    if (rtable == NULL || entry == NULL){
        perror("Erro null-pointer -> rtable_put erro 1");
        return -1;
    }
    EntryT * entry_t = malloc(sizeof(EntryT));
    if (entry_t == NULL){
        perror("Error malloc(entry) --> rtable_put");
        return -1;
    }
    entry_t__init(entry_t);
    entry_t->key = entry->key;
    entry_t->value.data = entry->value->data;
    entry_t->value.len = entry->value->datasize;

    if (rtable == NULL || entry_t == NULL){
        perror("Error malloc(entry) --> entry init");
        return -1;
    }

    MessageT *msg = wrap_message(MESSAGE_T__OPCODE__OP_PUT, MESSAGE_T__C_TYPE__CT_ENTRY);
    if (msg == NULL){
        perror("Error creating message in rtable_put");
        entry_t__free_unpacked(entry_t, NULL);
        return -1;
    }
        
    msg->entry = entry_t;
    // send a wait for response

    MessageT *received = network_send_receive(rtable, msg);

    if (received == NULL){
        perror("Error receiving response in rtable_put");
        message_t__free_unpacked(msg, NULL);
        entry_t__free_unpacked(entry_t, NULL);
        return -1;
    }
    if (received->c_type == MESSAGE_T__C_TYPE__CT_NONE && received->opcode == MESSAGE_T__OPCODE__OP_ERROR){   
        message_t__free_unpacked(msg, NULL);
        entry_t__free_unpacked(entry_t, NULL);
        fprintf(stderr, "Server returned error for put request\n");
        return -1;
    }
    return 0;
}


/* Retorna a entrada da tabela com chave key, ou NULL caso não exista
 * ou se ocorrer algum erro.
 */
struct block_t *rtable_get(struct rtable_t *rtable, char *key)
{
    if(key == NULL || rtable == NULL){
        perror("Erro null pointer-->rtable_get erro1");
        return NULL;
    }

    MessageT *msg = wrap_message(MESSAGE_T__OPCODE__OP_GET,MESSAGE_T__C_TYPE__CT_KEY);
    if (msg == NULL){
        perror("Error malloc(msg) --> rtable_get");
        return NULL;
    }

    msg->key = strdup(key);

    MessageT *received = network_send_receive(rtable, msg);

    if(received == NULL){
        perror("Error nullPointer --> received --> rtable_get");
        message_t__free_unpacked(msg, NULL);
        return NULL;
    }

    if(received->c_type == MESSAGE_T__C_TYPE__CT_NONE && received->opcode == MESSAGE_T__OPCODE__OP_ERROR){
        perror("Error received --> rtable_put");
        message_t__free_unpacked(received, NULL);
        return NULL;
    }

    struct block_t *block_obtido = unwrap_message(received);
    message_t__free_unpacked(received, NULL);

    return block_obtido;
    
}

/* Função para remover um elemento da tabela. Vai libertar
 * toda a memoria alocada na respetiva operação rtable_put().
 * Retorna 0 (OK), ou -1 (chave não encontrada ou erro).
 */
int rtable_del(struct rtable_t *rtable, char *key)
{
    if (rtable == NULL || key == NULL)
    {
        return -1; // Verifica se rtable ou key é NULL
    }

    // Aloca e inicializa a mensagem para remover a chave
    MessageT *request_message = (MessageT *)malloc(sizeof(MessageT));
    if (request_message == NULL)
    {
        return -1; // Verifica se a alocação da mensagem falhou
    }
    message_t__init(request_message);                    // Inicializa a mensagem
    request_message->opcode = MESSAGE_T__OPCODE__OP_DEL; // Define o opcode .... DELETE
    request_message->c_type = MESSAGE_T__C_TYPE__CT_KEY; // Define o tipo de mensagem
    request_message->key = strdup(key);                  // Duplica a chave

    // Envia a mensagem e aguarda a resposta
    MessageT *response_message = network_send_receive(rtable, request_message);
    free(request_message->key); // Libera a chave duplicada
    free(request_message);      // Libera a mensagem envolvida

    // Verifica a operação
    if (response_message == NULL ||
        (response_message->c_type == MESSAGE_T__C_TYPE__CT_NONE && response_message->opcode == MESSAGE_T__OPCODE__OP_ERROR))
    {
        if (response_message != NULL)
        {
            message_t__free_unpacked(response_message, NULL); // Libera a mensagem recebida
        }
        return -1;
    }

    message_t__free_unpacked(response_message, NULL); // Libera a mensagem recebida
    return 0;
}

/* Retorna o número de elementos contidos na tabela ou -1 em caso de erro.
 */
int rtable_size(struct rtable_t *rtable)
{
    if (rtable == NULL)
    {
        return -1; // Verifica se rtable é NULL
    }

    // Aloca e inicializa a mensagem para obter o tamanho da tabela
    MessageT *request_message = (MessageT *)malloc(sizeof(MessageT));
    if (request_message == NULL)
    {
        return -1;
    }
    message_t__init(request_message);                     // Inicializa a mensagem
    request_message->opcode = MESSAGE_T__OPCODE__OP_SIZE; // Define o opcode
    request_message->c_type = MESSAGE_T__C_TYPE__CT_NONE; // Define o tipo de mensagem

    // Envia a mensagem e aguarda a resposta
    MessageT *response_message = network_send_receive(rtable, request_message);
    free(request_message); // Libera a mensagem envolvida

    // Verifica a operação
    if (response_message == NULL ||
        (response_message->c_type == MESSAGE_T__C_TYPE__CT_NONE && response_message->opcode == MESSAGE_T__OPCODE__OP_ERROR))
    {
        if (response_message != NULL)
        {
            message_t__free_unpacked(response_message, NULL); // Libera a mensagem recebida
        }
        return -1;
    }

    int size = response_message->result;              // Obter o tamanho da tabela
    message_t__free_unpacked(response_message, NULL); // Libera a mensagem recebida
    return size;                                      // Retorna o tamanho da tabela
}

/* Retorna um array de char* com a cópia de todas as keys da tabela,
 * colocando um último elemento do array a NULL.
 * Retorna NULL em caso de erro.
 */
char **rtable_get_keys(struct rtable_t *rtable)
{
    // se hashtable é null
    if (rtable == NULL)
    {
        return NULL;
    }

    // Aloca e inicializa a mensagem para obter as chaves
    MessageT *request_message = (MessageT *)malloc(sizeof(MessageT));
    if (request_message == NULL)
    {
        return NULL;
    }
    message_t__init(request_message);                        // Inicializa a mensagem
    request_message->opcode = MESSAGE_T__OPCODE__OP_GETKEYS; // Define o opcode
    request_message->c_type = MESSAGE_T__C_TYPE__CT_NONE;    // Define o tipo de mensagem

    // Envia a mensagem e aguarda a resposta do servidor
    MessageT *response_message = network_send_receive(rtable, request_message);
    free(request_message);

    // Verifica a operação
    if (response_message == NULL ||
        (response_message->c_type == MESSAGE_T__C_TYPE__CT_NONE && response_message->opcode == MESSAGE_T__OPCODE__OP_ERROR))
    {
        if (response_message != NULL)
        {
            message_t__free_unpacked(response_message, NULL);
        }
        return NULL;
    }

    // Aloca buffer para as chaves
    char **key_array = (char **)malloc((response_message->n_keys + 1) * sizeof(char *));
    if (key_array == NULL)
    {
        message_t__free_unpacked(response_message, NULL);
        return NULL;
    }

    // Copia as chaves da mensagem para o buffer
    for (int i = 0; i < response_message->n_keys; i++)
    {
        key_array[i] = strdup(response_message->keys[i]);
        if (key_array[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(key_array[j]); // Libera as chaves já alocadas
            }
            free(key_array); // Libera o array de chaves
            message_t__free_unpacked(response_message, NULL);
            return NULL;
        }
    }
    key_array[response_message->n_keys] = NULL;       // Coloca o último elemento do array como NULL
    message_t__free_unpacked(response_message, NULL); // Libera a mensagem de resposta
    return key_array;                                 // Retorna o array de chaves
}
/* Liberta a memória alocada por rtable_get_keys().
 */
void rtable_free_keys(char **keys)
{
    if (keys == NULL)
    {
        return;
    }
    for (int index = 0; keys[index] != NULL; index++)
    {
        free(keys[index]);
    }
    free(keys);
}

/* Retorna um array de entry_t* com todo o conteúdo da tabela, colocando
 * um último elemento do array a NULL. Retorna NULL em caso de erro.
 */
struct entry_t **rtable_get_table(struct rtable_t *rtable)
{
    if (rtable == NULL)
    {
        return NULL;
    }
    MessageT *msg = wrap_message(MESSAGE_T__OPCODE__OP_GETTABLE, MESSAGE_T__C_TYPE__CT_NONE);
    if (msg == NULL)
    {
        return NULL;
    }
    MessageT *received = network_send_receive(rtable, msg);
    message_t__free_unpacked(msg, NULL);
    if (received == NULL)
    {
        return NULL;
    }
    struct entry_t **entries = malloc((received->n_entries + 1) * sizeof(struct entry_t *));
    if (entries == NULL)
    {
        message_t__free_unpacked(received, NULL);
        return NULL;
    }
    entries[received->n_entries] = NULL;

    for (int i = 0; i < received->n_entries; i++)
    {

        void *copied_data = malloc(received->entries[i]->value.len);
        if (copied_data == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                entry_destroy(entries[j]);
            }
            free(entries);
            message_t__free_unpacked(received, NULL);
            return NULL;
        }
        memcpy(copied_data, received->entries[i]->value.data, received->entries[i]->value.len);
        struct block_t *block = block_create(received->entries[i]->value.len, copied_data);
        if (block == NULL)
        {
            free(copied_data);
            for (int j = 0; j < i; j++)
            {
                entry_destroy(entries[j]);
            }
            free(entries);
            message_t__free_unpacked(received, NULL);
            return NULL;
        }
        struct entry_t *entry = entry_create(strdup(received->entries[i]->key), block);
        if (entry == NULL)
        {
            block_destroy(block);
            for (int j = 0; j < i; j++)
            {
                entry_destroy(entries[j]);
            }
            free(entries);
            message_t__free_unpacked(received, NULL);
            return NULL;
        }
        entries[i] = entry;
    }
    message_t__free_unpacked(received, NULL);
    return entries;
}

/* Liberta a memória alocada por rtable_get_table().
 */
void rtable_free_entries(struct entry_t **entries)
{
    if (entries == NULL)
    {
        return;
    }

    for (int index = 0; entries[index] != NULL; index++)
    {
        entry_destroy(entries[index]);
    }

    free(entries);
}

    
