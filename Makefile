CC = gcc
CFLAGS = -Wall -Werror -Wl,--gc-sections -O3 -Iinclude -flto -ffunction-sections -fdata-sections -g
STRIP_FLAGS = -s -R .comment -R .gnu.version --strip-unneeded

LIB_DIR = ./lib
BUILD_DIR = ./build

LIB_SRCS = $(wildcard $(LIB_DIR)/*.c)
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, main.c)
LIB_OBJS = $(patsubst $(LIB_DIR)/%.c, $(BUILD_DIR)/%.o, $(LIB_SRCS))

TARGET = lox

all: $(TARGET)

$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/main.o: main.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

strip: $(TARGET)
	strip $(STRIP_FLAGS) $(TARGET)

.PHONY: all clean strip
