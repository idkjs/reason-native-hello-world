let () = {
  let line: string = (Stdlib.read_line(): string);
  print_endline("Now unblocked!");
  ignore(line);
};
// ocamlfind opt -linkpkg -pp "refmt -p ml" -impl unblocked1.re && ./a.out