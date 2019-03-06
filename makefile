# Read the comments for compiling with macOS

CC = gcc
CFLAGS = -Wall -O3 -c
LDFLAGS = libisentlib.a  -lm -lglut -lGL -lX11
# For macOS, comment the preceding command line (put a # at the beginning)
# and uncomment the following command line:
#LDFLAGS = libisentlib.a -framework OpenGL -framework GLUT
TARGET = simulation

.PHONY: clean deepclean

$(TARGET): $(TARGET).o libisentlib.a positionFood.o evolution.o
	$(CC) -o $@.exe $^ $(LDFLAGS)

$(TARGET).o: $(TARGET).c definitions.h positionFood.h evolution.h
	$(CC) $(CFLAGS) $<


positionFood.o: positionFood.c definitions.h positionFood.h
	gcc $(CFLAGS) $<

evolution.o: evolution.c definitions.h evolution.h
	$(CC) $(CFLAGS) $< 


# isen libs ------------------------------------
libisentlib.a: BmpLib.o ErreurLib.o ESLib.o GfxLib.o OutilsLib.o SocketLib.o ThreadLib.o TortueLib.o VectorLib.o WavLib.o
	ar r libisentlib.a BmpLib.o ErreurLib.o ESLib.o GfxLib.o OutilsLib.o SocketLib.o ThreadLib.o TortueLib.o VectorLib.o WavLib.o
	ranlib libisentlib.a

BmpLib.o: BmpLib.c BmpLib.h OutilsLib.h
	gcc -Wall -O2 -c BmpLib.c

ESLib.o: ESLib.c ESLib.h ErreurLib.h
	gcc -Wall -O2 -c ESLib.c

ErreurLib.o: ErreurLib.c ErreurLib.h
	gcc -Wall -O2 -c ErreurLib.c

GfxLib.o: GfxLib.c GfxLib.h ESLib.h
	gcc -Wall -O2 -c GfxLib.c -I/usr/include/GL
# For macOS, comment the preceding command line (put a # at the beginning)
# and uncomment the following command line:
#	gcc -Wall -O2 -c GfxLib.c -Wno-deprecated-declarations

OutilsLib.o: OutilsLib.c OutilsLib.h
	gcc -Wall -O2 -c OutilsLib.c

SocketLib.o: SocketLib.c SocketLib.h
	gcc -Wall -O2 -c SocketLib.c

ThreadLib.o: ThreadLib.c ThreadLib.h
	gcc -Wall -O2 -c ThreadLib.c

TortueLib.o: TortueLib.c TortueLib.h GfxLib.h
	gcc -Wall -O2 -c TortueLib.c

VectorLib.o: VectorLib.c VectorLib.h
	gcc -Wall -O2 -c VectorLib.c -msse3

WavLib.o: WavLib.c WavLib.h OutilsLib.h
	gcc -Wall -O2 -c WavLib.c -Wno-unused-result
# For macOS, comment the preceding command line (put a # at the beginning)
# and uncomment the following command line:
#	gcc -Wall -O2 -c WavLib.c
# ----------------------------------------------

zip:
	tar -cvzf $(TARGET).tgz *.[ch] *.pdf makefile

clean:
	rm -f *~ *.o

deepclean: clean
	rm -f $(TARGET) libisentlib.a
