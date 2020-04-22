let () = {
  let line_promise: Lwt.t(string) = (
    Lwt_io.(read_line(stdin)): Lwt.t(string)
  );
  print_endline("Execution just continues...");
  ignore(line_promise);
};
// ocamlfind opt -linkpkg -package lwt.unix -pp "refmt -p ml" -impl unblocked_promise.re && ./a.out
// ocamlfind opt -linkpkg -package lwt.unix code.ml && ./a.out