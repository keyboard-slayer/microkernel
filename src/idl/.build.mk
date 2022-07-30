PROTO_OUT = ./src/idl
PROTO = $(PROTO_OUT)/src
PROTO_PY = $(wildcard src/pkgs/*/*.py)
PROTO_SRC = $(patsubst %.py, $(PROTO)/%.c, $(PROTO_PY))

IDL = PYTHONPATH=./src/idl:./$(basename $(1)):$$PYTHONPATH $(PY) -B $(1) $(PROTO_OUT)

$(PROTO)/%.c: %.py
	$(call IDL,$<)

$(PROTO): $(PROTO_SRC)