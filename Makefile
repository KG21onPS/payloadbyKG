PS4SDK ?= /opt/ps4sdk

LIBPS4 := $(PS4SDK)/libPS4

CC := gcc
OBJCOPY := objcopy

SRC_DIR := source
BUILD_DIR := build

TARGET := payloadbyKGtest.bin

INCLUDES := -I$(LIBPS4)/include

CFLAGS := $(INCLUDES) \
	-Os \
	-std=c11 \
	-ffunction-sections \
	-fdata-sections \
	-fno-builtin \
	-nostartfiles \
	-nostdlib \
	-Wall \
	-Wextra \
	-masm=intel \
	-march=btver2 \
	-mtune=btver2 \
	-m64 \
	-mabi=sysv \
	-mcmodel=small \
	-fpie \
	-fPIC

LDFLAGS := \
	-L$(LIBPS4) \
	-Xlinker -T \
	-Xlinker $(LIBPS4)/linker.x \
	-Wl,--build-id=none \
	-Wl,--gc-sections

SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(LIBPS4)/crt0.s $(OBJECTS) -o payload.elf \
		$(CFLAGS) $(LDFLAGS) -lPS4

	$(OBJCOPY) -O binary payload.elf $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) payload.elf $(TARGET)

.PHONY: all clean
