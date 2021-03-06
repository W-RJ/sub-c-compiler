# Global Makefile of SCC.
# Copyright (C) 2020-2021 Renjian Wang
#
# This file is part of SCC.
#
# SCC is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SCC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SCC.  If not, see <https://www.gnu.org/licenses/>.

# TODO: Windows
DESTDIR = /usr/local/bin

build = build
release = release

range = 1 2 3

ifeq ($(OS),Windows_NT)
    targets = "compiler/$(build)/scc.exe" "compiler/$(build)/sc.lang"
    ifneq ($(CG),4)
        targets += "interpreter/$(build)/sci.exe"
    endif
else
    targets = "compiler/$(build)/scc" "compiler/$(build)/sc.lang"
    ifneq ($(CG),4)
        targets += "interpreter/$(build)/sci"
    endif
endif

## phony targets

.PHONY : main release install uninstall zip test module_test all clean

main:
	"$(MAKE)" -C tools
	"$(MAKE)" -C common
	"$(MAKE)" -C interpreter
	"$(MAKE)" -C compiler
	mkdir -p "$(build)"
	cp $(targets) "$(build)" #TODO: Windows

release:
	"$(MAKE)" release=true -C tools
	"$(MAKE)" release=true -C common
	"$(MAKE)" release=true -C interpreter
	"$(MAKE)" release=true -C compiler
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
	zip -r scc.zip common compiler interpreter -x \*.pyc

test: module_test main
	export SCC='$(targets)'; \
	mkdir -p test/ncg; \
	pytest test/ncg; \
	RET=$$?; \
	if [ "$$RET" != "0" ] && [ "$$RET" != "5" ]; then \
		exit $$RET; \
	else \
		exit 0; \
	fi
	# TODO

module_test:
	"$(MAKE)" test -C tools
	"$(MAKE)" test -C common
	"$(MAKE)" unit_test -C compiler
	$(foreach i, $(range), "$(MAKE)" module_test CG=$(i) -C compiler &&) true
	# $(MAKE) module_test CG= -C compiler
	"$(MAKE)" test -C interpreter

# clean & rebuild
all: clean main

clean:
	"$(MAKE)" clean -C tools
	"$(MAKE)" clean -C common
	"$(MAKE)" clean -C interpreter
	"$(MAKE)" clean -C compiler
	-rm "$(build)/scc" "$(build)/sc.lang" "$(build)/sci"
	-rm scc.zip
