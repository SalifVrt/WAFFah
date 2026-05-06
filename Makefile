CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/main.c src/proxy.c src/waf.c src/logger.c
OBJ = $(SRC:.c=.o)
TARGET = waffah_bin

.PHONY: all clean format

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

format:
	clang-format -i src/*.c src/*.h
	@echo "code formatted successfully."

clean:
	rm -f src/*.o $(TARGET)