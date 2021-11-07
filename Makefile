CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c17
FUSE_FLAGS=-D_FILE_OFFSET_BITS=64
LDFLAGS=`pkg-config fuse --cflags --libs`
OBJFILES=$(wildcard src/*.c src/utils/*.c)
TARGET=fs

SCRIPTSDIR=scripts

DEST=/usr/local/bin

all: clean $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) $(FUSE_FLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

debug: CFLAGS += -DDEBUG
debug: $(TARGET)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -m 0777 $(TARGET) $(DEST)/$(TARGET)

check:
	@$(SCRIPTSDIR)/memcheck.bash $(TARGET)
