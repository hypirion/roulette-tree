CC=$(shell which gcc)
INCLUDEPATH= -Iinclude
CFLAGS+= -std=c99 -Wall $(COPT) ${INCLUDEPATH}

obj/%.o: src/%.o include/%.h
	mv src/$*.o obj/

clean:
	-rm -rf obj
purge: clean
	-rm -rf bin

bin:
	mkdir bin
obj:
	mkdir obj
