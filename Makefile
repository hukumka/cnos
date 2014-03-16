DEBUG=true
CFLAGS:=-Wall -fno-builtin -nostdinc -nostdlib -std=gnu99 -m32 -I./include
CPPFLAGS:=-Wall -fno-builtin -nostdinc -nostdlib -fno-exceptions -fno-rtti -m32 -I./include
CC:=gcc
CPP:=g++
ifeq ($(DEBUG), true)
	CFLAGS:=$(CFLAGS) -g3
	CPPFLAGS:=$(CPPFLAGS) -g3
else
	CFLAGS:=$(CFLAGS) -O2
	CPPFLAGS:=$(CPPFLAGS) -O2
endif
OBJFILES = \
	boot.o \
	kernel.o \
	common/pci.o \
	common/ata.o \
	common/memory.o
IMAGE = os.img

all: kernel.bin
kernel.bin: $(OBJFILES)
	@echo "Линковка ядра"
	@ld -T linker.ld -o $@ $^ -m elf_i386
.s.o:
	@echo "Компиляция "$<
	@as -o $@ $< --32
.c.o:
	@echo "Компиляция "$<
	@$(CC) $(CFLAGS) -o $@ -c $<
.cpp.o:
	@echo "Компиляция "$<
	@$(CPP) $(CPPFLAGS) -o $@ -c $< -include cpp.h
image: kernel.bin
	@echo "Создание образа"
	@dd if=/dev/zero of=$(IMAGE) bs=512 count=16065 1>/dev/null 2>>errors.log
	@echo "Создание FAT32 раздела"
	@losetup /dev/loop6 $(IMAGE) 
	@(echo c; echo u; echo n; echo p; echo 1; echo ; echo a; echo 1; echo t; echo c; echo w;) | fdisk /dev/loop6 1>/dev/null 2>>errors.log || true
	@losetup /dev/loop7 $(IMAGE) \
		--offset `echo \`fdisk -lu /dev/loop6 | sed -n 10p | awk '{print $$3}'\`*512 | bc` \
		--sizelimit `echo \`fdisk -lu /dev/loop6 | sed -n 10p | awk '{print $$4}'\`*512 | bc`
	@losetup -d /dev/loop6
	@mkdosfs /dev/loop7
	
	@echo "Записываем орех и grub на изображение"
	@mkdir -p tmpdir
	@mount /dev/loop7 tmpdir
	@mkdir tmpdir/boot
	@cp -r grub tmpdir/boot
	@cp kernel.bin tmpdir
	@sleep 1
	@umount /dev/loop7
	@rm -r tmpdir
	@losetup -d /dev/loop7
	
	@echo "Устанавливаем grub"
	@echo  "device (hd0) $(IMAGE)	\n \
			root (hd0,0)			\n \
			setup (hd0)				\n \
			quit\n" | grub --batch 1>/dev/null
	@echo "Готово"
	@-chown hukumka os.img
debug:
	@qemu-system-i386 os.img -s -S&
	@cgdb -x debugstart.gdb kernel.bin
	@killall qemu-system-i386
test:
	@qemu-system-i386 os.img
clear:
	@-rm $(OBJFILES) kernel.bin $(IMAGE) 
	@echo>errors.log
