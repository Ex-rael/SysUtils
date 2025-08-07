CC = gcc

SRC_DIR = src
BIN_DIR = bin

TARGET = $(BIN_DIR)/sys_handler

SRC = $(wildcard $(SRC_DIR)/*.c)

DEBUG_FLAGS = -Wall -Wextra -Wpedantic -g
RELEASE_FLAGS = -O3

CFLAGS = $(DEBUG_FLAGS)

.PHONY: all debug release run clean

all: debug

debug: CFLAGS = $(DEBUG_FLAGS)
debug: $(TARGET)

release: CFLAGS = $(RELEASE_FLAGS)
release: $(TARGET)

$(TARGET): $(SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BIN_DIR)

