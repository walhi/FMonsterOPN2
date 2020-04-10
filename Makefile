all: a.out

a.out: main.c vgm.c ym2612.c Makefile vgm.h
	gcc -g main.c vgm.c ym2612.c
