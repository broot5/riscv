CC = clang
CFLAGS = -Wall -Wextra -g -O2
TARGET = riscv
SRCS = main.c loader.c
OBJS = $(SRCS:.c=.o)
HEADERS = cpu.h handler.h instruction.h utils.h code_defines.h syscall.h loader.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
