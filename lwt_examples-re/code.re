let () = {
  let line_promise: Lwt.t(string) = (
    Lwt_io.(read_line(stdin)): Lwt.t(string)
  );
  print_endline("Execution just continues...");
  ignore(line_promise);
};
