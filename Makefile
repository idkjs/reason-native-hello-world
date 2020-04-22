.PHONY : examples

examples: examplesml examplesre
examplesml :
	dune build \

	  lwt_examples/force_wait.exe \

	  lwt_examples/hello_world.exe


		_build/default/lwt_examples/force_wait.exe

	  	_build/default/lwt_examples/hello_world.exe 
examplesre :
	dune build \
	  lwt_examples/code.exe \

	  lwt_examples/demo.exe 

	  	_build/default/lwt_examples/code.exe

		_build/default/lwt_examples/demo.exe 

	  	

INSTALL_ARGS := $(if $(PREFIX),--prefix $(PREFIX),)

default:
	dune build

install:
	dune install $(INSTALL_ARGS)

uninstall:
	dune uninstall $(INSTALL_ARGS)

reinstall: uninstall install

clean:
	dune clean

.PHONY: default install uninstall reinstall clean