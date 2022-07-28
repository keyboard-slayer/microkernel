KERNEL = $(BUILD)/kernel.elf
KERNEL_BUILD = $(BUILD)/kernel

KERNEL_CFLAGS = 					\
	$(CFLAGS)						\
	-fno-pic						\
	-fno-pie						\
	-mno-red-zone					\
	-mno-sse						\
	-mno-sse2						\
	-fno-zero-initialized-in-bss	\
	-mcmodel=kernel					\
	-m64							\
	-DSCHED_QUANTUM=$(SCHED_QUANTUM)\
	-D__kernel__

KERNEL_LDFLAGS +=					\
	$(LDFLAGS)						\
	-z max-page-size=0x1000

KERNEL_SRC += 						\
	$(LIBC_SRC)						\
	$(wildcard src/kernel/src/*.c)	\
	$(wildcard src/liballoc/*.c)

KERNEL_OBJ := $(patsubst %, $(KERNEL_BUILD)/%.o, $(KERNEL_SRC))
DEPENDENCIES += $(KERNEL_OBJ:.o=.d)

$(KERNEL_BUILD)/%.c.o: %.c
	@$(MKCWD)
	$(CC) -c -o $@ $< $(KERNEL_CFLAGS)

$(KERNEL_BUILD)/%.s.o: %.s
	@$(MKCWD)
	$(AS) -o $@ $< $(ASFLAGS)

$(KERNEL): $(KERNEL_OBJ)
	@$(MKCWD)
	$(LD) -o $@ $^ $(KERNEL_LDFLAGS)
