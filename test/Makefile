CC=$(shell which gcc)
LIBPATH= -I../include
INCLUDEPATH= $(LIBPATH) -Iinclude
CFLAGS+= -std=gnu99 -Wall $(COPT) ${INCLUDEPATH} -g -DRTREE_DEBUG
OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/,main.o rtree.o)

EXEC = bin/main

$(OBJDIR)/rtree.o: ../src/rtree.c ../include/rtree.h
	@$(MAKE) -C .. -q $(OBJDIR)/rtree.o || \
   $(MAKE) --no-print-directory -C .. $(OBJDIR)/rtree.o
	cp ../$(OBJDIR)/rtree.o $(OBJDIR)/

$(OBJDIR)/%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXEC)

test: $(EXEC)
	@$(EXEC)

$(EXEC): $(OBJS) bin
	$(CC) $(OBJS) -o $(EXEC)

clean:
	-rm -rf $(OBJDIR)
purge: clean
	-rm -rf bin

$(OBJS): | $(OBJDIR)

bin:
	mkdir bin
$(OBJDIR):
	mkdir $(OBJDIR)
