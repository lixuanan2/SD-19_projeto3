
/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */

//head
#include "table.h"
#include "table-private.h"
#include "server_network.h"
#include "server_skeleton.h"
#include "htmessages.pb-c.h"
#include "message.h"
//
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h> 
#include <signal.h> 


/* Função para preparar um socket de receção de pedidos de ligação
* num determinado porto.
* Retorna o descritor do socket ou -1 em caso de erro.
*/
int server_network_init(short port){

    signal(SIGPIPE, SIG_IGN); //*//

    // Cria socket TCP
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0 ) {
        perror("server_network_init erro 1, Erro ao criar socket");
        return -1;
    }

    // Preenche estrutura server com endereço(s) para associar (bind) à socket 
    struct sockaddr_in serv_addr; // :server address
    serv_addr.sin_family = AF_INET; // ipv4
    serv_addr.sin_port = htons(port); // Porta TCP
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Todos os endereços na máquina

    //*// SO_REUSEADDR
    int opt = 1;
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("server_network_init erro 2, Erro ao definir SO_REUSEADDR");
        close(server_sockfd);
        return -1;
    }

    // Faz bind
    if (bind(server_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("server_network_init erro 3, Erro ao fazer bind");
        close(server_sockfd);
        return -1;
    }

    // Esta chamada diz ao SO que esta é uma socket para receber pedidos
    if (listen(server_sockfd, 0) < 0){
        perror("server_network_init erro 4, Erro ao executar listen");
        close(server_sockfd);
        return -1;
    }

    return server_sockfd;
}

/* A função network_main_loop() deve:
* - Aceitar uma conexão de um cliente;
* - Receber uma mensagem usando a função network_receive;
* - Entregar a mensagem de-serializada ao skeleton para ser processada
na tabela table;
* - Esperar a resposta do skeleton;
* - Enviar a resposta ao cliente usando a função network_send.
* A função não deve retornar, a menos que ocorra algum erro. Nesse
* caso retorna -1.
*/
int network_main_loop(int listening_socket, struct table_t *table){

    //printf("Servidor à espera de dados\n");
    printf("Server ready, waiting for connections\n");

    struct sockaddr_in cli_addr; // client address
    socklen_t size_client = sizeof(cli_addr);

    while(1){
        int client_sockfd = accept(listening_socket, (struct sockaddr *) &cli_addr, &size_client); 
        if (client_sockfd == -1) {
            perror("network_main_loop erro 1, Erro ao aceitar conexão1"); 
            continue; //OR return -1?
        }

        printf("Client connection established\n");

        while(1){
            MessageT *request_msg = network_receive(client_sockfd);
            if (request_msg == NULL) {
                //perror("network_main_loop erro 2, network_recive");
                break;
            }

            int ivk = invoke(request_msg, table); // server_skeleton.invoke()
            if (ivk < 0) {
                perror("network_main_loop erro 3, invoke");
                message_t__free_unpacked(request_msg, NULL);
                break;
            }
            
            int send = network_send(client_sockfd, request_msg);
            if (send < 0) {
                perror("network_main_loop erro 4, network_send");
                message_t__free_unpacked(request_msg, NULL);
                break;
            }

            message_t__free_unpacked(request_msg, NULL);
            }

        printf("Client connection closed\n");

        printf("Server ready, waiting for connections\n");
        close(client_sockfd); 

    }
    return -1;
}

/* A função network_receive() deve:
* - Ler os bytes da rede, a partir do client_socket indicado;
* - De-serializar estes bytes e construir a mensagem com o pedido,
* reservando a memória necessária para a estrutura MessageT.
* Retorna a mensagem com o pedido ou NULL em caso de erro.
*/
MessageT *network_receive(int client_socket){
    return read_message(client_socket);
}



/* A função network_send() deve:
* - Serializar a mensagem de resposta contida em msg;
* - Enviar a mensagem serializada, através do client_socket.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int network_send(int client_socket, MessageT *msg){
    return send_message(client_socket,msg);
}



/* Liberta os recursos alocados por server_network_init(), nomeadamente
* fechando o socket passado como argumento.
* Retorna 0 (OK) ou -1 em caso de erro.
*/
int server_network_close(int socket){
    if(close(socket) == -1){
        perror("server_network_close erro");
        return -1;
    }
    return 0;
}
