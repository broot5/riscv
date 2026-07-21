.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

ifeq ($(origin CC), default)
CC := clang
endif

CPPFLAGS += -Iinclude
CFLAGS += -Wall -Wextra -Wshadow -g -O2 -MMD -MP
LDFLAGS ?=
LDLIBS ?=

BUILD_DIR := build
HOST_BUILD_DIR := $(BUILD_DIR)/host
TEST_BUILD_DIR := $(BUILD_DIR)/tests
TARGET := $(BUILD_DIR)/riscv

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,$(HOST_BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

RISCV_CC ?= clang
RISCV_CPPFLAGS := -Itests/include
RISCV_ASFLAGS := --target=riscv32-unknown-elf -march=rv32imc -mabi=ilp32
RISCV_LDFLAGS := -nostdlib -static -fuse-ld=lld -Wl,-T,tests/link.ld

TEST_SRCS := $(wildcard tests/*.S)
TEST_ELFS := $(patsubst tests/%.S,$(TEST_BUILD_DIR)/%.elf,$(TEST_SRCS))
LOADER_TEST := $(TEST_BUILD_DIR)/loader_validation
ENCODING_TEST := $(TEST_BUILD_DIR)/encoding_validation
DECODER_TEST := $(TEST_BUILD_DIR)/decoder_validation
HOST_TESTS := $(LOADER_TEST) $(ENCODING_TEST) $(DECODER_TEST)

all: $(TARGET)

$(TARGET): $(OBJS) | check-host-tools
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(HOST_BUILD_DIR)/%.o: src/%.c | $(HOST_BUILD_DIR) check-host-tools
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(TEST_BUILD_DIR)/%.elf: tests/%.S tests/include/test_macros.inc tests/link.ld | $(TEST_BUILD_DIR) check-test-tools
	$(RISCV_CC) $(RISCV_CPPFLAGS) $(RISCV_ASFLAGS) $< $(RISCV_LDFLAGS) -o $@

$(LOADER_TEST): tests/loader_validation.c $(HOST_BUILD_DIR)/loader.o $(HOST_BUILD_DIR)/cpu.o $(HOST_BUILD_DIR)/memory.o | $(TEST_BUILD_DIR) check-host-tools
	$(CC) $(CPPFLAGS) $(filter-out -MMD -MP,$(CFLAGS)) -o $@ $^

$(ENCODING_TEST): tests/encoding_validation.c $(HOST_BUILD_DIR)/compressed_decoder.o $(HOST_BUILD_DIR)/cpu.o $(HOST_BUILD_DIR)/fetch.o $(HOST_BUILD_DIR)/memory.o $(HOST_BUILD_DIR)/utils.o | $(TEST_BUILD_DIR) check-host-tools
	$(CC) $(CPPFLAGS) $(filter-out -MMD -MP,$(CFLAGS)) -o $@ $^

$(DECODER_TEST): tests/decoder_validation.c $(HOST_BUILD_DIR)/cpu.o $(HOST_BUILD_DIR)/decoder.o $(HOST_BUILD_DIR)/instructions.o $(HOST_BUILD_DIR)/instructions_m.o $(HOST_BUILD_DIR)/memory.o $(HOST_BUILD_DIR)/syscall.o $(HOST_BUILD_DIR)/utils.o | $(TEST_BUILD_DIR) check-host-tools
	$(CC) $(CPPFLAGS) $(filter-out -MMD -MP,$(CFLAGS)) -o $@ $^

$(HOST_BUILD_DIR) $(TEST_BUILD_DIR):
	mkdir -p $@

check-host-tools:
	sh scripts/check-tools.sh $(firstword $(CC))

check-test-tools: check-host-tools
	sh scripts/check-tools.sh $(firstword $(RISCV_CC)) ld.lld

test: $(TARGET) $(TEST_ELFS) $(HOST_TESTS)
	sh tests/run-tests.sh $(TARGET) $(TEST_BUILD_DIR)
	$(LOADER_TEST)
	$(ENCODING_TEST)
	$(DECODER_TEST)

clean:
	rm -rf build

-include $(DEPS)

.PHONY: all test clean check-host-tools check-test-tools
