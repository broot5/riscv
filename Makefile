CC = clang
CFLAGS = -Wall -Wextra -Wshadow -g -O2 -Iinclude
TARGET = riscv
SRCS = src/main.c src/loader.c
OBJS = $(SRCS:.c=.o)
HEADERS = include/cpu.h include/instructions.h include/decoder.h include/utils.h include/opcodes.h include/syscall.h include/loader.h include/compressed_decoder.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
