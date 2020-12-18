build = build

range = 1 2

## phony targets

.PHONY : main zip test module_test all clean

main:
	$(MAKE) -C tools
	$(MAKE) -C scc
	mkdir -p $(build)
	cp scc/$(build)/scc scc/$(build)/sc.lang $(build)

release:
	$(MAKE) release=true -C tools
	$(MAKE) release=true -C scc
	mkdir -p $(build)
	cp scc/$(build)/scc scc/$(build)/sc.lang $(build)

zip: release
	-rm scc.zip
	zip -r scc.zip scc -x \*.pyc

test: module_test main
	# TODO

module_test:
	$(MAKE) test -C tools
	$(MAKE) unit_test -C scc
	$(foreach i, $(range), $(MAKE) module_test CG=$(i) -C scc;)
	# $(MAKE) module_test CG= -C scc

# clean & rebuild
all: clean main

clean:
	$(MAKE) clean -C tools
	$(MAKE) clean -C scc
	-rm $(build)/scc $(build)/sc.lang
	-rm scc.zip
