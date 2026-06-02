#!/usr/bin/bash
mkdir -p build/

c++ $(pkg-config --cflags sdl3) -Wall -Wextra -ggdb src/main.cpp -o build/main $(pkg-config --libs sdl3)
