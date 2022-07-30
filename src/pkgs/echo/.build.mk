ECHO = $(BUILD)/echo.elf
ECHO_SRC = $(wildcard src/pkgs/echo/src/*.c) $(LIBC_SRC) src/protocol/src/echo.c
BUILD_ECHO = $(BUILD)/echo
ECHO_OBJ = $(patsubst %, $(BUILD_ECHO)/%.o, $(ECHO_SRC))
ECHO_CFLAGS = $(CFLAGS) -mno-sse -mno-sse2

$(BUILD_ECHO)/%.c.o: %.c
	@$(MKCWD)
	$(CC) $(ECHO_CFLAGS) -c -o $@ $<

$(BUILD_ECHO)/%.s.o: %.s
	@$(MKCWD)
	$(AS) -o $@ $< $(ASFLAGS)

$(ECHO): $(ECHO_OBJ)
	@$(MKCWD)
	$(LD) $(LDFLAGS) -o $@ $^ -T src/pkgs/link.ld

echo: $(ECHO)
	cp $(ECHO) $(SYSROOT)/bin/echo.elf
	@bash -c 'echo -e "\nMODULE_PATH=boot:///bin/echo.elf\n" >> $(SYSROOT)/boot/limine.cfg'
