main:
	$(MAKE) -C tools
	$(MAKE) -C scc

## phony targets

.PHONY : test all clean

test: main
	$(MAKE) test -C tools
	$(MAKE) test -C scc
	# TODO

# clean & rebuild
all: clean main

clean:
	$(MAKE) clean -C tools
	$(MAKE) clean -C scc
