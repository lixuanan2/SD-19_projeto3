/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_stub.h"
#include "entry.h"
#include "block.h"

void print_usage() {
    printf("Usage: table-client <server>:<port>\n");
}

void print_usage2() {
    printf("Usage: ");
    printf("p[ut] <key> <value> |");
    printf(" g[et] <key> |");
    printf(" d[el] <key> |");
    printf(" s[ize] |");
    printf(" k[eys] |");
    printf(" t[able] |");
    printf(" q[uit]\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Invalid args!\n");
        print_usage();
        return -1;
    }

    struct rtable_t *rtable = rtable_connect(argv[1]);
    if (rtable == NULL) {
        fprintf(stderr, "Error in connect\n");
        return -1;
    }

    char command[256];
    printf("Command: ");
    while (fgets(command, 256, stdin) != NULL) {
        char *cmd = strtok(command, " \n");
        
        if (cmd == NULL) {
            printf("Invalid command.\n");
            print_usage2();
        } else if (strcmp(cmd, "put") == 0 || strcmp(cmd, "p") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "\n");

            if (key == NULL || value == NULL) {
                printf("Invalid command.\n");
                print_usage2();
            } else {
                struct entry_t *entry = entry_create(strdup(key), block_create(strlen(value) + 1, strdup(value)));
                if (rtable_put(rtable, entry) == 0) {
                    printf("%s :: %s\n", key,value);
                } else {
                    printf("Failed to put entry.\n");
                }
                entry_destroy(entry);
            }
        } else if (strcmp(cmd, "get") == 0 || strcmp(cmd, "g") == 0) {
            char *key = strtok(NULL, " \n");
            if (key == NULL) {
                printf("Invalid command.\n");
                print_usage2();
            } else {
                struct block_t *block = rtable_get(rtable, key);
                if (block != NULL) {
                    printf("%s\n", (char *)block->data);
                    block_destroy(block);
                } else {
                    printf("Key not found.\n");
                }
            }
        } else if (strcmp(cmd, "del") == 0 || strcmp(cmd, "d") == 0) {
            char *key = strtok(NULL, " \n");
            if (key == NULL) {
                printf("Invalid command.\n");
                print_usage2();
            } else {
                if (rtable_del(rtable, key) == 0) {
                    printf("Entry removed\n");
                } else {
                    printf("Failed to remove entry.\n");
                }
            }
        } else if (strcmp(cmd, "size") == 0 || strcmp(cmd, "s") == 0) {
            int size = rtable_size(rtable);
            if (size >= 0) {
                printf("Table size: %d\n", size);
            } else {
                printf("Failed to get table size.\n");
            }
        } else if (strcmp(cmd, "keys") == 0 || strcmp(cmd, "k") == 0) {
            char **keys = rtable_get_keys(rtable);
            if (keys != NULL) {
                for (int i = 0; keys[i] != NULL; i++) {
                     if (keys[i + 1] != NULL) {
                        printf("%s\n", keys[i]);
                    } else {
                        printf("%s", keys[i]);
                    }
                    free(keys[i]);
                }       
                printf("\n");
                free(keys);
            } else {
                printf("Failed to get keys.\n");
            }
        } else if (strcmp(cmd, "table") == 0 || strcmp(cmd, "t") == 0) {
            struct entry_t **entries = rtable_get_table(rtable);
            if (entries != NULL) {
                for (int i = 0; entries[i] != NULL; i++) {
                    printf("%s : %s\n", entries[i]->key, (char *)entries[i]->value->data);
                    entry_destroy(entries[i]);
                }
                free(entries);
            } else {
                printf("Failed to get table entries.\n");
            }
        } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0) {
            printf("Bye, bye!\n");
            break;
        } else {
            printf("Invalid command.\n");
            print_usage2();
        }
        printf("Command: ");
    }

    rtable_disconnect(rtable);
    return 0;
}
