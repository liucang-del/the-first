# 《数据结构（C语言版 第2版）》目录知识点演示程序
CC      ?= cc
CFLAGS  ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS = -Iinclude
LDFLAGS ?=
LDLIBS  ?= -lm

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
BIN = ds-demo

.PHONY: all run test asan clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

src/%.o: src/%.c include/common.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

run: $(BIN)
	./$(BIN)

test: $(BIN)
	./$(BIN) > build-output.txt
	@tail -n 4 build-output.txt

asan:
	$(CC) $(CPPFLAGS) -std=c11 -O1 -g -fsanitize=address,undefined \
		-fno-omit-frame-pointer -o $(BIN)-asan $(SRC) $(LDLIBS)
	./$(BIN)-asan > /dev/null

clean:
	rm -f $(OBJ) $(BIN) $(BIN)-asan build-output.txt
