CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -Iinclude
LDFLAGS = -Wl,--gc-sections
DYLIBS = -lm -lunistring -lpcre2-8 -lreadline -lz -lbrotlienc -lbrotlidec -llzf -llz4 -llzma -lzstd

LIB_DIR = lib
STD_DIR = std
OBJECT_DIR = $(STD_DIR)/objects
MODULE_DIR = $(STD_DIR)/modules
BUILD_DIR = build

LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
STD_SRCS = $(wildcard $(STD_DIR)/*.c)
OBJECT_SRCS = $(wildcard $(OBJECT_DIR)/*.c)
MODULE_SRCS = $(wildcard $(MODULE_DIR)/*.c)

MAIN_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, main.c)
LIB_OBJS = $(patsubst $(LIB_DIR)/%.c, $(BUILD_DIR)/%.o, $(LIB_SRCS))
STD_OBJS = $(patsubst $(STD_DIR)/%.c, $(BUILD_DIR)/%.o, $(STD_SRCS))           \
           $(patsubst $(OBJECT_DIR)/%.c, $(BUILD_DIR)/%.o, $(OBJECT_SRCS))     \
           $(patsubst $(MODULE_DIR)/%.c, $(BUILD_DIR)/%.o, $(MODULE_SRCS))

TARGET = lox

all: release

debug: CFLAGS += -g -O0 -DDEBUG
debug: $(TARGET)

release: LDFLAGS += -s -flto=auto
release: CFLAGS += -Ofast -flto=auto -ffunction-sections -fdata-sections
release: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(LIB_OBJS) $(STD_OBJS)
	$(CC) $(LDFLAGS) -o $@ build/*.o $(DYLIBS)

$(BUILD_DIR)/main.o: main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(STD_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(OBJECT_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(MODULE_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

fmt:
	clang-format -i main.c lib/*.c std/*.c include/*.h

lint:
	clang-tidy std/*.c lib/*.c include/*.h -- -Iinclude

rebuild: clean release

.PHONY: all clean fmt rebuild lint debug release
