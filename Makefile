ENTRY_POINT := 0xc0001500
PRIMARY_HD := hd60M.img

C_SRCS := $(shell find ./src -type f -name '*.c')
C_OBJS := $(C_SRCS:.c=.o)
C_OBJS := $(C_OBJS:./src/%=./build/%)
C_DEPS := $(C_OBJS:.o=.d)
CC := gcc
CFLAGS := -m32 -Wall -I./include -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -fno-stack-protector 
 
AS_SRCS := $(shell find ./src -type f -name '*.S')
AS_SRCS := $(filter-out ./src/boot/mbr.S ./src/boot/loader.S, $(AS_SRCS))
AS_OBJS := $(AS_SRCS:.S=.o)
AS_OBJS := $(AS_OBJS:./src/%=./build/%)
AS_DEPS := $(AS_OBJS:.o=.d)
AS := nasm
ASFLAGS := -f elf32
ASBINLIB := -I ./include

LD := ld
# LDFLAGS := -m elf_i386 -Ttext $(ENTRY_POINT) -e main --strip-all
LDFLAGS := -m elf_i386 -Ttext $(ENTRY_POINT) -e main

MBR_BIN := ./build/boot/mbr.bin
LOADER_BIN := ./build/boot/loader.bin
KERNEL_BIN := ./build/kernel/kernel.bin

SRC_DIRS := $(shell find ./src/ -type d)
BUILD_DIRS := $(SRC_DIRS:./src/%=./build/%)

all: mkdirs binary deploy

mkdirs:
	@mkdir -p $(BUILD_DIRS)

./build/%.d: ./src/%.c
	@set -e; \
	mkdir -p $$(dirname ./build/$*); \
	rm -f $@; \
	$(CC) $(CFLAGS) -MM $< -MT ./build/$*.o > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	echo './build/$*.o: ; $(CC) $(CFLAGS) -c -o ./build/$*.o $< && objcopy --remove-section .note.gnu.property ./build/$*.o' >> $@; \
	rm -f $@.$$$$

./build/%.d: ./src/%.S
	@set -e; \
	mkdir -p $$(dirname ./build/$*); \
	rm -f $@; \
	python3 asmdep.py -I ./include -MT ./build/$*.o $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	echo './build/$*.o: ; $(AS) $(ASFLAGS) -o ./build/$*.o $<' >> $@; \
	rm -f $@.$$$$

sinclude $(C_DEPS)
sinclude $(AS_DEPS)

$(KERNEL_BIN) : $(AS_OBJS) $(C_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(MBR_BIN) : src/boot/mbr.S include/boot/boot.inc
	$(AS) -I ./include -o $@ $<

$(LOADER_BIN) : src/boot/loader.S include/boot/boot.inc
	$(AS) -I ./include -o $@ $<

binary: $(MBR_BIN) $(LOADER_BIN) $(KERNEL_BIN)

deploy:
	dd if=$(MBR_BIN) of=$(PRIMARY_HD) bs=512 count=1 conv=notrunc
	dd if=$(LOADER_BIN) of=$(PRIMARY_HD) bs=512 count=4 seek=2 conv=notrunc
	dd if=$(KERNEL_BIN) of=$(PRIMARY_HD) bs=512 count=200 seek=9 conv=notrunc

clean:
	rm -f $(C_OBJS) $(AS_OBJS) $(C_DEPS) $(AS_DEPS) $(KERNEL_BIN) $(MBR_BIN) $(LOADER_BIN)

create-hd:
	@. ./env.sh || exit; bximage -hd -mode="flat" -size=60 -q $(PRIMARY_HD)

remove-hd:
	rm -f $(PRIMARY_HD)

