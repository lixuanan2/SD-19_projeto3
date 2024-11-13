#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>  // for htonl and ntohl

#include "message.h"
#include "htmessages.pb-c.h"
#include "entry.h"
#include "block.h"

// read_all 
ssize_t read_all(int sock, void *buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t res = read(sock, buf + total, len - total);
        if (res < 0) {
            if (errno == EINTR) continue;
            perror("read failed:");
            return -1;
        }
        if (res == 0){
            //printf("Connection closed by peer during read.\n");
            return total;
        } 
        total += res;
    }
    
    return total;
}

// write_all 
ssize_t write_all(int sock, void *buf, size_t len) {
    size_t size_len = len;
    while (len > 0) {
        ssize_t res = write(sock, buf, len);
        if (res < 0) {
            if (errno == EINTR) continue;
            perror("write failed:");
            return -1;
        }
        if (res == 0) return 0;
        buf += res;
        len -= res;
    }
    return size_len;
}

MessageT *wrap_message(MessageT__Opcode opcode, MessageT__CType ctype) {
    MessageT *msg = malloc(sizeof(MessageT));
    if (msg == NULL){
        perror("Error malloc(msg) --> wrap_message");
        return NULL;
    }
    message_t__init(msg);
    msg->opcode = opcode;
    msg->c_type = ctype;
    return msg;
}

struct block_t *unwrap_message(MessageT* msg) {
    if (msg == NULL || msg->value.data == NULL) {
        perror("Error nullPointer --> unwrap_message");
        return NULL;
    }

    void* value = duplicate_memory(msg->value.data, msg->value.len);

    if(value == NULL){
        perror("Error value --> unwrap_message");
        return NULL;
    }

    // Create a block from msg->value
    struct block_t *block = block_create(msg->value.len, value);
    if (block == NULL) {
        perror("Error creating block in unwrap_message");
        free(value);
        return NULL;
    }

    return block;
}

MessageT *read_message(int sockfd) {
    uint32_t msg_size_32;
    if (read_all(sockfd, &msg_size_32, sizeof(msg_size_32)) != sizeof(msg_size_32)) {
        //perror("Error reading message size in read_message");
        return NULL;
    }

    size_t msg_size = ntohl(msg_size_32);

    void* buffer = (uint8_t *)malloc(msg_size);
    if (buffer == NULL) {
        perror("Error allocating buffer in read_message");
        return NULL;
    }

    if (read_all(sockfd, buffer, msg_size) != msg_size) {
        perror("Error reading full message in read_message");
        free(buffer);
        return NULL;
    }

    // Deserialize message
    MessageT *msg = message_t__unpack(NULL, msg_size, buffer);
    //free(buffer);

    if (msg == NULL) {
        fprintf(stderr, "Error deserializing message in read_message\n");
    }

    return msg;
}

int send_message(int sockfd, MessageT *msg) {
    if (msg == NULL) {
        fprintf(stderr, "Null message provided to send_message\n");
        return -1;
    }

    size_t msg_size = message_t__get_packed_size(msg);

    uint32_t network_msg_size = htonl(msg_size);

    uint8_t *buffer = (uint8_t *)malloc(msg_size * sizeof(uint8_t *));
    if (buffer == NULL) {
        perror("Error allocating buffer in send_message");
        return -1;
    }

    // Serialize message
    message_t__pack(msg, buffer);

    if (write_all(sockfd, (uint8_t *)&network_msg_size, sizeof(network_msg_size)) != sizeof(network_msg_size)) {
        perror("Error sending message size in send_message");
        free(buffer);
        return -1;
    }

    if (write_all(sockfd, (void *)buffer, msg_size) != msg_size) {
        perror("Error sending message content in send_message");
        free(buffer);
        return -1;
    }

    free(buffer);
    return 0;
}
