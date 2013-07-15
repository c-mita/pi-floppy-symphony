CC=g++
CFLAGS=-g -I../include
IDIR = ../include
ODIR = obj
BDIR = ../bin
LIBS = -lm -lwiringPi

_DEPS = floppy.h midi.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = floppy.o single.o midi.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

single: $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
