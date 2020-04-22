let () =
  Lwt_main.run(
    {
      let%lwt data = Lwt_io.(read_line(stdin));
      let%lwt () = Lwt_io.printl(data);
      Lwt.return();
    },
  );