
#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>   

#include "message-private.h"

#include "htmessages.pb-c.h"

ssize_t read_all(int sock, void *buf, size_t len);

//int read_all(int sock, void *buf, int len);

ssize_t write_all(int sock, void *buf, size_t len);

//int write_all(int sock, char *buf, int len);

MessageT* wrap_message(MessageT__Opcode opcode, MessageT__CType ctype);

struct block_t * unwrap_message(MessageT* msg);

MessageT *read_message(int client_socket);

int send_message(int client_socket, MessageT *msg);

#endif