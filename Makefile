CC=$(shell which gcc)
INCLUDEPATH= -Iinclude
CFLAGS+= -std=gnu99 -Wall $(COPT) ${INCLUDEPATH}

obj/%.o: src/%.o include/%.h obj
	mv src/$*.o obj/

clean:
	-rm -rf obj
purge: clean
	-rm -rf bin

bin:
	mkdir bin
obj:
	mkdir obj
