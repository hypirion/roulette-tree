GRAPHS=$(wildcard *.dot)
PNGS=$(patsubst %.dot,%.png,$(GRAPHS))

.PHONY: all clear purge

all: $(PNGS)

clear:
	-rm -f $(PNGS)

purge: clear
	-rm -f $(GRAPHS)

%.png:
	dot $*.dot -Tpng -o $@
