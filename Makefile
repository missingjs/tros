C_SRCS := $(shell find ./src -type f -name '*.c')
C_OBJS_TMP := $(C_SRCS:.c=.o)
C_OBJS := $(C_OBJS_TMP:./src/%=./build/%)

AS_SRCS := $(shell find ./src -type f -name '*.S')
AS_OBJS_TMP := $(AS_SRCS:.S=.o)
AS_OBJS := $(AS_OBJS_TMP:./src/%=./build/%)

AS := nasm
CC := gcc
LD := ld

CFLAGS := -m32 -Wall -I./include -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -fno-stack-protector 
ASFLAGS = -f elf32

TARGET := ./build/kernel/kernel.bin

DEPS_TMP := $(C_SRCS:.c=.d)
DEPS := $(DEPS_TMP:./src/%=build/%)
SRC_DIRS := $(shell find ./src/ -type d)
BUILD_DIRS := $(SRC_DIRS:./src/%=./build/%)

all: $(BUILD_DIRS) $(TARGET)

$(BUILD_DIRS):
	@mkdir -p $@

build/%.d: ./src/%.c
	@set +e; mkdir -p $$(dirname ./build/$*); rm -f $@; \
	$(CC) $(CFLAGS) -MM $< > $@.$$$$; \
	sed 's,\(.*\)\.o[ :]*,./build/$*.o $@ : ,g' < $@.$$$$ > $@; \
	echo '	$(CC) $(CFLAGS) -c -o build/$*.o $<' >> $@; \
	rm -f $@.$$$$

sinclude $(DEPS)

$(TARGET) : $(C_OBJS)
	$(CC) $(CFLAGS) -o $@ $(C_OBJS)




clean:
	rm -f $(C_OBJS) $(TARGET) $(DEPS)
