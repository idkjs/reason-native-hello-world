# OS := $(shell uname -s)

# all: unix

# unix:
# 	@ocamlc demo.ml -o demo.exe
# 	@rm demo.cmo demo.cmi
.PHONY: run
run: _build/default/code.exe
	_build/default/code.exe

_build/default/code.exe: $(wildcard *.re) $(wildcard *.rei)
	dune build code.exe