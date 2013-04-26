CC=$(shell which gcc)
INCLUDEPATH= -Iinclude
CFLAGS+= -std=gnu99 -Wall $(COPT) ${INCLUDEPATH} -g -DRTREE_DEBUG
OBJS = obj/main.o obj/rtree.o

obj/%.o: src/%.o include/%.h obj
	mv src/$*.o obj/

all: $(OBJS)
	$(CC) $(OBJS) -o bin/main

clean:
	-rm -rf obj
purge: clean
	-rm -rf bin

bin:
	mkdir bin
obj:
	mkdir obj
