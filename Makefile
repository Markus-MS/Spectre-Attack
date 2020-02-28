all: x86

x86: main_x86

main_x86: main.c
	gcc main.c -o spectre -Os
