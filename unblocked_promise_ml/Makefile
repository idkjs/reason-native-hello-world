# OS := $(shell uname -s)

# all: unix

# unix:
# 	@ocamlc demo.ml -o demo.exe
# 	@rm demo.cmo demo.cmi
.PHONY: run

all: code force_wait
code: _build/default/code.exe
	_build/default/code.exe

_build/default/code.exe: $(wildcard *.ml) $(wildcard *.mli)
	dune build code.exe
force_wait: _build/default/force_wait.exe
	_build/default/force_wait.exe

_build/default/force_wait.exe: $(wildcard *.ml) $(wildcard *.mli)
	dune build force_wait.exe