CC=gcc
CFLAGS=-I.
DEPS = ptask.h
LIBS = -lm -lpthread -Wall `allegro-config --cflags --libs`
OBJ = main.o ptask.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $/*.o *~ core $(INCDIR)/*~
