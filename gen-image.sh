#!/usr/bin/env sh

if [ "$#" -ne 1 ] || ! [ -f "$1" ]; then
  echo "Usage: $0 <KERNEL_FILE>"
  exit 1
fi

if [ ! -d ./tmp ]; then
	mkdir ./tmp
fi

if [ ! -d ./tmp/boot/ ]; then
	mkdir ./tmp/boot
fi

if [ ! -d ./tmp/boot/grub/ ]; then
	mkdir ./tmp/boot/grub/
fi

if [ ! -f ./tmp/boot/grub/grub.cfg ]; then
	echo "set timeout=5" >> ./tmp/boot/grub/grub.cfg
	echo "set default=0" >> ./tmp/boot/grub/grub.cfg
	echo "menuentry \"kitos\" {" >> ./tmp/boot/grub/grub.cfg
  echo "    multiboot2 /boot/kitos.elf" >> ./tmp/boot/grub/grub.cfg
  echo "    boot" >> ./tmp/boot/grub/grub.cfg
	echo "}" >> ./tmp/boot/grub/grub.cfg

	echo "menuentry \"kitos-vga\" {" >> ./tmp/boot/grub/grub.cfg
  echo "    multiboot2 /boot/kitos.elf -vga" >> ./tmp/boot/grub/grub.cfg
  echo "    boot" >> ./tmp/boot/grub/grub.cfg
	echo "}" >> ./tmp/boot/grub/grub.cfg
fi

cp $1 ./tmp/boot/kitos.elf

grub-mkrescue -o kitos.iso ./tmp
