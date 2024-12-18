#!/bin/sh

qemu-system-x86_64 kitos.iso -serial stdio -m 12G -d int --no-reboot -device pci-bridge,chassis_nr=1  -device pci-bridge,chassis_nr=3,bus=pci.1 -device megasas,bus=pci.1,id=foo -device e1000,bus=pci.1,netdev=net0 -netdev user,id=net0 -device pci-ohci,bus=pci.2 -device piix4-usb-uhci,bus=pci.2 -device usb-ehci,bus=pci.2 -device qemu-xhci,bus=pci.2 -device ich9-usb-uhci6,bus=pci.2 -smp 4
