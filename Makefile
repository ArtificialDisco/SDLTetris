RMF = rm -f

CC = gcc `sdl-config --libs` -lm -lSDL_image
CO = gcc `sdl-config --cflags` -c

SOURCES = tetris.c \
		  options.c \
		  drawing.c \
		  init.c \
		  useful.c \
		  button.c \
		  rules.c

HEADERS = tetris.h \
		  options.h \
		  drawing.h \
		  init.h \
		  useful.h \
		  button.h \
		  rules.h

OBJECTS = tetris.o \
		  options.o \
		  drawing.o \
		  init.o \
		  useful.o \
		  button.o \
		  rules.o

TARGET = tetris

.SUFFIXES: .c
.c.o:
	$(CO) $< -o $@

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

clean:
	$(RMF) $(OBJECTS) $(TARGET)

tetris.o: tetris.c tetris.h options.h button.h init.h useful.h
rules.o: rules.c rules.h init.h useful.h
options.o: options.c options.h tetris.h
drawing.o: drawing.c drawing.h
button.o: button.c button.h useful.h
init.o: init.c init.h tetris.h useful.h
useful.o: useful.h
