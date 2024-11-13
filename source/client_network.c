/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h> 
//
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#include "client_network.h"
#include "client_stub.h"
#include "client_stub-private.h"
#include "htmessages.pb-c.h"
#include "message.h"


/* Esta função deve:
 * - Obter o endereço do servidor (struct sockaddr_in) com base na
 * informação guardada na estrutura rtable;
 * - Estabelecer a ligação com o servidor;
 * - Guardar toda a informação necessária (e.g., descritor do socket)
 * na estrutura rtable;
 * - Retornar 0 (OK) ou -1 (erro).
 */
int network_connect(struct rtable_t *rtable)
{   
    signal(SIGPIPE, SIG_IGN); //*//

    if( rtable->server_address == NULL || rtable->server_port <= 0){
        perror("Erro na information invalid of rtable --> client_network_connect erro 1\n");
        return -1;
    }
    // criar socket
    if((rtable->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Erro ao criar socket TCP--> client_network_connect erro 2\n");
        return -1;
    }

    // preencer as informacoes necessarias
    struct sockaddr_in server_addr;
    server_addr.sin_port = htons(rtable->server_port);
    server_addr.sin_family = AF_INET;
    // usa inet_pton() para garantir que os endereco IP converte to formato binary
    if (inet_pton(AF_INET, rtable->server_address,&server_addr.sin_addr) < 1) {
        perror("Erro ao converter IP --> client_network_connect erro 3\n");
        network_close(rtable);
        return -1;
    }

    // estabelece a ligacao com o servidor
    if(connect(rtable->sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
        perror("Erro ao estabelecer a ligacao com o servidor --> client_network_connect erro 4\n");
        network_close(rtable);
        return -1;
    }
    
    return 0;
}

/* Esta função deve:
 * - Obter o descritor da ligação (socket) da estrutura rtable_t;
 * - Serializar a mensagem contida em msg;
 * - Enviar a mensagem serializada para o servidor;
 * - Esperar a resposta do servidor;
 * - De-serializar a mensagem de resposta;
 * - Tratar de forma apropriada erros de comunicação;
 * - Retornar a mensagem de-serializada ou NULL em caso de erro.
 */
MessageT *network_send_receive(struct rtable_t *rtable, MessageT *msg)
{
    // Verifica se são válidos
    if (rtable == NULL || msg == NULL || rtable->sockfd < 0)
    {
        return NULL;
    }

    // Envia a mensagem serializada para o servidor
    if (send_message(rtable->sockfd, msg) < 0)
    {
        return NULL;
    }
    // Lê a resposta do servidor e de-serializa a mensagem
    return read_message(rtable->sockfd);
}

/* Fecha a ligação estabelecida por network_connect().
 * Retorna 0 (OK) ou -1 (erro).
 */
int network_close(struct rtable_t *rtable)
{
    if (rtable == NULL)
    {
        return -1;
    }
    // Fecha o socket se estiver aberto
    if (rtable->sockfd >= 0)
    {
        close(rtable->sockfd);
        rtable->sockfd = -1;
    }

    return 0;
}
