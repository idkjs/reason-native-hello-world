# HelloWorld ReasonML Native

1. Install [opam](https://opam.ocaml.org/)
2. Install `reason` language options with `opam install reason`
3. Create a `hello.re` file:

```reason
print_string("Hello world!\n");
```

4. Compile `hello.re` with `ocamlc -o hello -pp "refmt -p ml" -impl hello.re`.

5. Open your terminal and run `./hello`.

## Source

[https://riptutorial.com/ocaml/example/7096/hello-world](https://riptutorial.com/ocaml/example/7096/hello-world)
