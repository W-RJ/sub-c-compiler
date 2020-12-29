# TODO: Windows
DESTDIR = /usr/local/bin

build = build
release = release

range = 1 2 3

targets = "scc/$(build)/scc" "scc/$(build)/sc.lang"
ifneq ($(CG),4)
    targets += "sci/$(build)/sci"
endif

## phony targets

.PHONY : main release install uninstall zip test module_test all clean

main:
	"$(MAKE)" -C tools
	"$(MAKE)" -C common
	"$(MAKE)" -C sci
	"$(MAKE)" -C scc
	mkdir -p "$(build)"
	cp $(targets) "$(build)" #TODO: Windows

release:
	"$(MAKE)" release=true -C tools
	"$(MAKE)" release=true -C common
	"$(MAKE)" release=true -C sci
	"$(MAKE)" release=true -C scc
	mkdir -p "$(build)" "$(release)"
	cp $(targets) "$(build)"
	cp $(targets) "$(release)"

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
	"$(MAKE)" clean -C sci
	"$(MAKE)" clean -C scc
	-rm "$(build)/scc" "$(build)/sc.lang" "$(build)/sci"
	-rm scc.zip
