CC=g++
CFLAGS=-I.
DEPS = LedDisplayPi.h

%.o: %.c $(DEPS)
	&(CC) -c -o $@ $< $(CFLAGS)

print: LedDisplayPi.o print.o
	$(CC) -o print %^