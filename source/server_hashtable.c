/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "server_network.h"
#include "server_skeleton.h"
#include "table.h"

// quit com ctrl c
void handle_sigint(int sig) {
    exit(0);
}

int main(int argc, char *argv[]) {
    // check comando do terminal
    if (argc != 3) {
        printf("Usage: %s <port> <n_lists>\n", argv[0]);
        return -1;
    }

    short port = atoi(argv[1]);
    int n_lists = atoi(argv[2]);

    // Set up signal handling for graceful shutdown
    signal(SIGINT, handle_sigint);

    // Initialize the server skeleton and hash table
    struct table_t *table = server_skeleton_init(n_lists);
    if (table == NULL) {
        perror("Error initializing table");
        return -1;
    }

    // Initialize server network
    int server_socket = server_network_init(port);
    if (server_socket < 0) {
        perror("Error initializing network");
        server_skeleton_destroy(table);
        return -1;
    }

    //printf("Server ready, waiting for connections\n");

    // Enter the main loop to accept and handle client requests
    if (network_main_loop(server_socket, table) < 0) {
        perror("Error in main loop");
    }

    // close
    server_network_close(server_socket);
    server_skeleton_destroy(table);
    
    printf("Server shutdown completed\n");
    return 0;
}
