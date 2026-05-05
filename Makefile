CC = gcc
CFLAGS = -Wall -Wextra -g
SRC = src/main.c src/proxy.c src/waf.c src/logger.c
OBJ = $(SRC:.c=.o)
TARGET = waffah_bin

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f src/*.o $(TARGET)