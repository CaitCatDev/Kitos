.POSIX:
.SUFFIXES: .c .s .o

CC=clang
AS=clang
LD=ld.lld
CFLAGS=-I ./includes -I ./includes/arch/x86 -mno-sse -mno-sse2 -fno-PIE -fno-PIC -ffreestanding -mno-red-zone -mcmodel=large

COBJS=./src/start.o ./src/pci-class.o ./src/main.o ./src/drivers/ide.o ./src/drivers/pci.o ./src/drivers/serial.o ./src/kstdio.o ./src/arch/x86/idt.o ./src/arch/x86/isr.o ./src/lib/memory.o ./src/arch/x86/interrupts.o ./src/lib/string.o ./src/mmu/pmm.o ./src/mmu/vmm.o
KERNEL=kernel.elf

all: $(KERNEL)

./pci.ids:
	curl https://pci-ids.ucw.cz/v2.2/pci.ids -o $@

./pci-gen: ./utils/pci-gen.c
	$(CC) -o $@ ./utils/pci-gen.c

./src/pci-class.c: pci.ids ./pci-gen
	./pci-gen pci.ids $@ ./includes/pci-class.h

$(KERNEL): $(COBJS)
	$(LD) -T ./ld-scripts/os.ld $(COBJS) -o $@

.s.o:
	$(AS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(COBJS) $(KERNEL)
