# TODO: Windows
DESTDIR = /usr/local/bin

build = build
release = release

range = 1 2 3 5

## phony targets

.PHONY : main release install uninstall zip test module_test all clean

main:
	"$(MAKE)" -C tools
	"$(MAKE)" -C common
	"$(MAKE)" -C scc
	"$(MAKE)" -C sci
	mkdir -p "$(build)"
	cp "scc/$(build)/scc" "scc/$(build)/sc.lang" "sci/$(build)/sci" "$(build)" #TODO: Windows

release:
	"$(MAKE)" release=true -C tools
	"$(MAKE)" release=true -C common
	"$(MAKE)" release=true -C scc
	"$(MAKE)" release=true -C sci
	mkdir -p "$(build)" "$(release)"
	cp "scc/$(build)/scc" "scc/$(build)/sc.lang" "sci/$(build)/sci" "$(build)"
	cp "scc/$(build)/scc" "scc/$(build)/sc.lang" "sci/$(build)/sci" "$(release)"

install: release
	mkdir -p "$(DESTDIR)"
	cp "$(release)/scc" "$(release)/sc.lang" "$(release)/sci" -t "$(DESTDIR)"

uninstall:
	rm "$(DESTDIR)/scc" "$(DESTDIR)/sci" "$(DESTDIR)/sc.lang"

zip: release
	-rm scc.zip
	zip -r scc.zip common scc sci -x \*.pyc

test: module_test main
	# TODO

module_test:
	"$(MAKE)" test -C tools
	"$(MAKE)" test -C common
	"$(MAKE)" unit_test -C scc
	$(foreach i, $(range), "$(MAKE)" module_test CG=$(i) -C scc &&) true
	# $(MAKE) module_test CG= -C scc
	"$(MAKE)" test -C sci

# clean & rebuild
all: clean main

clean:
	"$(MAKE)" clean -C tools
	"$(MAKE)" clean -C common
	"$(MAKE)" clean -C scc
	"$(MAKE)" clean -C sci
	-rm "$(build)/scc" "$(build)/sc.lang" "$(build)/sci"
	-rm scc.zip
