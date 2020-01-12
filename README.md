# HelloWorld ReasonML Native

A note to self on using `OCaml` with the `ReasonML` syntax without `Bucklescript`.

## Setup

1. Install [opam](https://opam.ocaml.org/)
2. Install `reason` language options with `opam install reason`

## HelloWorld

Create a `hello.re` file:

```ocaml
// hello.re
print_string("Hello world!\n");
```

Compile `hello.re` by running `ocamlc -o hello -pp "refmt -p ml" -impl hello.re`.

Open your terminal and run `./hello`.

Your output is:

```sh
➜  helloworld ocamlc -o hello -pp "refmt -p ml" -impl hello.re
➜  helloworld ./hello
Hello world!
```

## Sources

[https://riptutorial.com/ocaml/example/7096/hello-world](https://riptutorial.com/ocaml/example/7096/hello-world)

[repo](https://github.com/idkjs/reason-native-hello-world)
