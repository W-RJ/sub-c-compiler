build = build

## phony targets

.PHONY : main zip test all clean

main:
	$(MAKE) -C tools
	$(MAKE) -C scc
	mkdir -p $(build)
	cp scc/$(build)/scc scc/$(build)/sc.lang $(build)

zip: main
	-rm scc.zip
	zip -r scc.zip scc

test: main
	$(MAKE) test -C tools
	$(MAKE) test -C scc
	# TODO

# clean & rebuild
all: clean main

clean:
	$(MAKE) clean -C tools
	$(MAKE) clean -C scc
	-rm $(build)/scc $(build)/sc.lang
	-rm scc.zip
