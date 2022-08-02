TTY = $(BUILD)/tty.elf
TTY_SRC = $(wildcard src/pkgs/tty/src/*.c) $(LIBC_SRC) 
BUILD_TTY = $(BUILD)/tty
TTY_OBJ = $(patsubst %, $(BUILD_TTY)/%.o, $(TTY_SRC))
TTY_CFLAGS = $(CFLAGS) -mno-sse -mno-sse2

$(BUILD_TTY)/%.c.o: %.c
	@$(MKCWD)
	$(CC) $(TTY_CFLAGS) -c -o $@ $<

$(BUILD_TTY)/%.s.o: %.s
	@$(MKCWD)
	$(AS) -o $@ $< $(ASFLAGS)

$(TTY): $(TTY_OBJ)
	@$(MKCWD)
	$(LD) $(LDFLAGS) -o $@ $^ -T src/pkgs/link.ld

tty: $(TTY)
	cp $(TTY) $(SYSROOT)/bin/tty.elf
	@bash -c 'echo -e "\nMODULE_PATH=boot:///bin/tty.elf\n" >> $(SYSROOT)/boot/limine.cfg'
