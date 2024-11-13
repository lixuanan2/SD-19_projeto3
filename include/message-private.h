#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

// message-private.h
ssize_t read_all(int sock, void *buf, size_t len);

//int read_all(int sock, void *buf, int len);

ssize_t write_all(int sock, void *buf, size_t len);

//int write_all(int sock, char *buf, int len);

#endif