# Jiarong Li 58177
# Xiaoji Wu 59800
# Jiaxin Huang 60808

CC = gcc
CFLAGS = -Wall -g -I $(INC_DIR) -MMD -MP
LDFLAGS = -L$(LIB_DIR) -ltable -lprotobuf-c

BIN_DIR = binary
INC_DIR = include
OBJ_DIR = object
SRC_DIR = source
LIB_DIR = library
DEP_DIR = dependencies

# Static objects from project 1
STATIC_OBJECTS = $(OBJ_DIR)/block.o $(OBJ_DIR)/entry.o $(OBJ_DIR)/list.o $(OBJ_DIR)/table.o

# Project 2 objects
PROJECT_OBJECTS = $(OBJ_DIR)/server_network.o $(OBJ_DIR)/server_skeleton.o $(OBJ_DIR)/client_stub.o $(OBJ_DIR)/client_network.o $(OBJ_DIR)/htmessages.pb-c.o $(OBJ_DIR)/message.o

# Library
LIBTABLE = $(LIB_DIR)/libtable.a

# Targets
.PHONY: all libtable client_hashtable server_hashtable clean
all: libtable $(BIN_DIR)/client_hashtable $(BIN_DIR)/server_hashtable

# libtable.a
libtable: $(STATIC_OBJECTS) | $(LIB_DIR)
	ar -rcs $(LIBTABLE) $(STATIC_OBJECTS)

# client_hashtable binary
$(BIN_DIR)/client_hashtable: $(OBJ_DIR)/client_hashtable.o $(PROJECT_OBJECTS) $(LIBTABLE)
	$(CC) $(CFLAGS) -o $@ $(OBJ_DIR)/client_hashtable.o $(PROJECT_OBJECTS) $(LDFLAGS)

# server_hashtable binary
$(BIN_DIR)/server_hashtable: $(OBJ_DIR)/server_hashtable.o $(PROJECT_OBJECTS) $(LIBTABLE)
	$(CC) $(CFLAGS) -o $@ $(OBJ_DIR)/server_hashtable.o $(PROJECT_OBJECTS) $(LDFLAGS)

# Compile hashtable source files
$(OBJ_DIR)/client_hashtable.o: $(SRC_DIR)/client_hashtable.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/client_hashtable.d -c $(SRC_DIR)/client_hashtable.c -o $@

$(OBJ_DIR)/server_hashtable.o: $(SRC_DIR)/server_hashtable.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/server_hashtable.d -c $(SRC_DIR)/server_hashtable.c -o $@

# Compile static objects from Project 1
$(OBJ_DIR)/block.o: $(SRC_DIR)/block.c $(INC_DIR)/block.h | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/block.d -c $< -o $@

$(OBJ_DIR)/entry.o: $(SRC_DIR)/entry.c $(INC_DIR)/entry.h $(INC_DIR)/block.h | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/entry.d -c $< -o $@

$(OBJ_DIR)/list.o: $(SRC_DIR)/list.c $(INC_DIR)/block.h $(INC_DIR)/entry.h | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/list.d -c $< -o $@

$(OBJ_DIR)/table.o: $(SRC_DIR)/table.c $(INC_DIR)/table.h $(INC_DIR)/list.h $(INC_DIR)/block.h | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/table.d -c $< -o $@

# Compile project 2 objects with dependencies
$(OBJ_DIR)/server_network.o: $(SRC_DIR)/server_network.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/server_network.d -c $(SRC_DIR)/server_network.c -o $@

$(OBJ_DIR)/server_skeleton.o: $(SRC_DIR)/server_skeleton.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/server_skeleton.d -c $(SRC_DIR)/server_skeleton.c -o $@

$(OBJ_DIR)/client_stub.o: $(SRC_DIR)/client_stub.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/client_stub.d -c $(SRC_DIR)/client_stub.c -o $@

$(OBJ_DIR)/client_network.o: $(SRC_DIR)/client_network.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/client_network.d -c $(SRC_DIR)/client_network.c -o $@

$(OBJ_DIR)/htmessages.pb-c.o: $(SRC_DIR)/htmessages.pb-c.c | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/htmessages.pb-c.d -c $(SRC_DIR)/htmessages.pb-c.c -o $@

$(OBJ_DIR)/message.o: $(SRC_DIR)/message.c $(INC_DIR)/message.h $(INC_DIR)/message-private.h | $(OBJ_DIR) $(DEP_DIR)
	$(CC) $(CFLAGS) -MF $(DEP_DIR)/message.d -c $(SRC_DIR)/message.c -o $@

# Dependency management
-include $(DEP_DIR)/*.d

# clean all
clean:
	rm -f $(OBJ_DIR)/*.o $(LIBTABLE) $(BIN_DIR)/client_hashtable $(BIN_DIR)/server_hashtable
	rm -rf $(DEP_DIR)/*.d
