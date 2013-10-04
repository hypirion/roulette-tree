CC=$(shell which gcc)
CFLAGS+= -std=gnu99 -Wall $(COPT) ${INCLUDEPATH} -g -DRTREE_DEBUG
OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/,main.o rtree.o)
SUBDIRS = test

EXEC = bin/main

.PHONY: test

$(OBJDIR)/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@

all: $(EXEC)

$(EXEC): $(OBJS) bin
	$(CC) $(OBJS) -o $(EXEC)

test: $(OBJS)
	@$(MAKE) --no-print-directory -C test test

clean:
	-rm -rf $(OBJDIR)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done
purge: clean
	-rm -rf bin
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir purge; \
	done

$(OBJS): | $(OBJDIR)

bin:
	mkdir bin
$(OBJDIR):
	mkdir $(OBJDIR)
