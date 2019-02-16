all: build

build: anti_aliasing.c anti_aliasing.h main.c
	gcc -o anti_aliasing anti_aliasing.c main.c -lpthread -Wall -lm

run:
	./anti_aliasing test.pnm out.pnm 4 4

.PHONY: clean
clean:
	rm -f anti_aliasing
	rm -rf out/*
