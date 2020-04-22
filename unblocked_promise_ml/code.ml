let () =
  let line_promise : string Lwt.t =
    Lwt_io.(read_line stdin) in
  print_endline "Execution just continues...";
  ignore line_promise