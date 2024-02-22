CC = gcc
CFLAGS = -Wall -Ofast -Iinclude -flto=auto -ffunction-sections -fdata-sections
LDFLAGS = -flto=auto -Wl,--gc-sections -s

LIB_DIR = ./lib
STD_DIR = ./std
BUILD_DIR = ./build

LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
STD_SRCS = $(wildcard $(STD_DIR)/*.c)

MAIN_OBJ = $(patsubst %.c, $(BUILD_DIR)/%.o, main.c)
LIB_OBJS = $(patsubst $(LIB_DIR)/%.c, $(BUILD_DIR)/%.o, $(LIB_SRCS))
STD_OBJS = $(patsubst $(STD_DIR)/%.c, $(BUILD_DIR)/%.o, $(STD_SRCS))

TARGET = lox

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(LIB_OBJS) $(STD_OBJS)
	$(CC) $(LDFLAGS) -o $@ build/*.o -lm

$(BUILD_DIR)/main.o: main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(STD_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

fmt:
	clang-format -i main.c lib/*.c std/*.c include/*.h

.PHONY: all clean
