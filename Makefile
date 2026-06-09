CC := c++

CFLAGS := $(shell pkg-config --cflags sdl3) \
          -Wall \
          -Wextra \
          -ggdb

LIBS := $(shell pkg-config --libs sdl3)

main:
	mkdir -p build
	$(CC) $(CFLAGS) src/main.cpp -o build/main $(LIBS)

clean:
	rm -rf build

