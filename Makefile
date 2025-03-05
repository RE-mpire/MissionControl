# Makefile for Networking project

CXX = clang++
CXXFLAGS = -Wall -std=c++11
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

CLIENT_SRC = $(SRC_DIR)/client.cpp
MAIN_SRC = $(SRC_DIR)/main.cpp

CLIENT_OBJ = $(OBJ_DIR)/client.o
MAIN_OBJ = $(OBJ_DIR)/main.o

CLIENT_BIN = $(BIN_DIR)/client
MAIN_BIN = $(BIN_DIR)/server

all: $(CLIENT_BIN) $(MAIN_BIN)

$(CLIENT_BIN): $(CLIENT_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(MAIN_BIN): $(MAIN_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(CLIENT_OBJ): $(CLIENT_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean