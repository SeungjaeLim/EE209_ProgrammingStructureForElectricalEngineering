CC=gcc209
CCFLAGS= -D_DEFAULT_SOURCE -D_GNU_SOURCE
%.o: %.c
	$(CC) $(CCFLAGS) -c $<
all: ish
clobber: clean
		 rm -f *~ \#*\# core
clean:
		rm -f *.o

ish: ish.o dynarray.o execute.o lexical.o scheduler.o signal.o syntatic.o
	$(CC) $(CCFLAGS) -o $@ ish.o lexical.o syntatic.o signal.o scheduler.o dynarray.o execute.o
ish.o: dynarray.h lexical.h syntatic.h scheduler.h signal.h execute.h
dynarray.o: dynarray.h
execute.o: dynarray.h lexical.h syntatic.h scheduler.h signal.h execute.h
lexical.o : dynarray.h lexical.h syntatic.h scheduler.h signal.h execute.h
scheduler.o : dynarray.h lexical.h syntatic.h scheduler.h signal.h execute.h
signal.o : dynarray.h lexical.h syntatic.h scheduler.h signal.h execute.h
syntatic.o : dynarray.h lexical.h syntatic.h scheduler.h signal.h execute.h
