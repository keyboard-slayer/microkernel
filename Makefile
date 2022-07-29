CC = clang
LD = ld.lld
AS = nasm
PY ?= python3

MKCWD = mkdir -p $(@D)
BUILD = ./build

LOADER ?= limine
ARCH ?= x86_64
BIOS ?= /usr/share/ovmf/x64/OVMF.fd
SYSROOT ?= ./sysroot
SCHED_QUANTUM ?= 8

CFLAGS =							\
	-std=c2x						\
	-fno-builtin					\
	-pedantic						\
	-Wpedantic						\
	-Werror							\
	-Wextra							\
	-Wall							\
	-Wno-gnu-empty-struct			\
	-Wno-gnu-statement-expression	\
	-ggdb							\
	-fno-stack-protector			\
	-Os								\
	-D__$(ARCH)__					\
	-DSTB_SPRINTF_IMPLEMENTATION	\
	-Isrc/libc/inc					\
	-Isrc/

ASFLAGS =							\
	-F dwarf						\
	-g

LDFLAGS = 							\
	-static							\
	-nostdlib						\

QEMU_FLAGS =												\
	-no-shutdown											\
	-no-reboot												\
	-serial mon:stdio										\
	-m 256M													\
	-smp 4													\
	-bios $(BIOS)											\
	-drive file=fat:rw:sysroot,media=disk,format=raw		\
	-enable-kvm												\
	
include src/protocol/.build.mk
include src/unittests/.build.mk
include src/$(ARCH)/.build.mk
include src/$(LOADER)/.build.mk
include src/libc/.build.mk
include src/kernel/.build.mk
include src/pkgs/.build.mk

run: sysroot
	$(QEMU) $(QEMU_FLAGS)

test: $(UNITTEST)
	$(UNITTEST)

proto: $(PROTO)

sysroot: $(KERNEL) $(LOADER_FILE)
	mkdir -p $(SYSROOT)/boot $(SYSROOT)/bin
	cp $(KERNEL) $(SYSROOT)/boot

clean:
	rm -rf $(BUILD)
	rm -rf $(SYSROOT)

.PHONY: run clean
.DEFAULT_GOAL := sysroot