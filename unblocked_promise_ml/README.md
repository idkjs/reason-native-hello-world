# [lwt-counter-server](https://www.baturin.org/code/lwt-counter-server/)

## Running

Build it with:

```sh
ocamlfind ocamlopt -package lwt,lwt.unix, -linkpkg -o demo ./demo.re
```

Start the executable, and start a few telnet sessions to 127.0.0.1:9000 to test it.

## unblocked_promise

In order run unblocked promise you have to have `libev` on your machine because
`conf-libev` required by `lwt` requires it. [docs](https://github.com/ocsigen/lwt#installing).

`brew install libev` then `opam install conf-libev lwt`.