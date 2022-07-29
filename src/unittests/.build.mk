UNITTEST = $(BUILD)/unittest.elf
UNITTEST_BUILD = $(BUILD)/unittest
UNITTEST_CC ?= gcc

UNITTEST_CFLAGS = 				\
	-std=c2x					\
	-Wall						\
	-Werror						\
	-Werror						\
	-Wextra						\
	-ggdb						\
	-fno-stack-protector		\
	-fsanitize=address			\
	-fsanitize=undefined		\
	-lcmocka

UNITTEST_SRC = $(wildcard src/unittests/*.c)			\
			   src/libc/src/map.c 					    \
			   src/libc/src/vec.c 						\
			   src/libc/src/reader.c					\
			   src/libc/src/json.c

$(UNITTEST): $(UNITTEST_SRC)
	@$(MKCWD)
	$(UNITTEST_CC) $^ -o $@ $(UNITTEST_CFLAGS)
