CC = clang
CFLAGS = -Wall -Wextra -std=c11 -g -z separate-loadable-segments -z max-page-size=4096 -march=armv8.4-a -lm
LDFLAGS = -fuse-ld=lld
TARGET = write-protect
SRCS = write-protect.c memfd-helpers.c range-map.c signal-helpers.c bss-helpers.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

globals: all
	nm $(TARGET) | grep ' global_\| range_map\| atomic_var'
loads: all
	llvm-readelf -Wl $(TARGET) | grep 'LOAD'

bss: all
	llvm-readelf -WS $(TARGET) | grep '.bss'

maps: all
	cat /proc/`ps -A -o pid,comm | grep $(TARGET) | awk '{ print $$1 }'`/maps | grep '$(TARGET)\|bss_memfd'

run: all
	./$(TARGET)