# OS := $(shell uname -s)

# all: unix

# unix:
# 	@ocamlc demo.ml -o demo.exe
# 	@rm demo.cmo demo.cmi
.PHONY : examples

examples: examplesml examplesre

examplesml :
	dune build \

	  ./force_wait.exe 

	  ./hello_world.exe


		./_build/default/lwt_examples/force_wait.exe

	  	./_build/default/lwt_examples/hello_world.exe 
examplesre :
	dune build \
	  ./code.exe \

	  ./demo.exe 

	  	./_build/default/lwt_examples/code.exe

		./_build/default/lwt_examples/demo.exe 