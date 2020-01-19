# [lwt-counter-server](https://www.baturin.org/code/lwt-counter-server/)

## Running

Build it with:

```sh
ocamlfind ocamlopt -package lwt,lwt.unix,logs,logs.lwt -linkpkg -o counter-server ./counter-server.re
```

Start the executable, and start a few telnet sessions to 127.0.0.1:9000 to test it.