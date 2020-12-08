build = build

main:
	$(MAKE) -C tools
	$(MAKE) -C scc
	mkdir -p $(build)
	cp scc/$(build)/scc $(build)

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
	-rm $(build)/scc
