CC=$(shell which gcc)
INCLUDEPATH= -Iinclude
CFLAGS+= -std=gnu99 -Wall $(COPT) ${INCLUDEPATH} -g -DRTREE_DEBUG
OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/,main.o rtree.o)

EXEC = bin/main

$(OBJDIR)/%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXEC)

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
