CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -O3 -fuse-ld=mold -ljansson
TARGET  = lang

SRCDIR   = src
OBJDIR   = build

SRC_FILES = $(shell find $(SRCDIR) -name '*.c')
OBJ_FILES = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC_FILES))

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@echo "Linking..."
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJDIR)

.PHONY: clean
