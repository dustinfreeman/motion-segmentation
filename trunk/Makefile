CC = gcc
CFLAGS = -g -Wall -O3 -march=pentium-m
OBJECTS = main.o math.o tiff.o globals.o levelset.o motion.o
INCFLAGS = -I/usr/local/include
LDFLAGS = -Wl -L/usr/local/lib
LIBS = -lm -lpthread -ltiff 

all: main

main: $(OBJECTS)
	$(CC) -o motion $(OBJECTS) $(LDFLAGS) $(INCFLAGS) $(LIBS)

.SUFFIXES:
.SUFFIXES:	.c .cc .C .cpp .o

.c.o :
	$(CC) -o $@ -c $(CFLAGS) $< $(INCFLAGS)

count:
	wc *.c *.cc *.C *.cpp *.h *.hpp

clean:
	rm -f *.o

.PHONY: all
.PHONY: count
.PHONY: clean
