/** Multi-client server example.

    Clients can increment a shared counter or read its current value.

    Build with: ocamlfind ocamlopt -package lwt,lwt.unix,logs,logs.lwt -linkpkg -o counter-server ./counter-server.ml
 */;

open Lwt;

/* Shared mutable counter */
let counter = ref(0);

let listen_address = Unix.inet_addr_loopback;

let port = 9000;

let backlog = 10;

let handle_message = (msg) =>
  switch msg {
  | "read" => string_of_int(counter^)
  | "inc" =>
    counter := counter^ + 1;
    "Counter has been incremented";
  | _ => "Unknown command"
  };

let rec handle_connection = (ic, oc, ()) =>
  Lwt_io.read_line_opt(ic)
  >>= (
    (msg) =>
      switch msg {
      | Some(msg) =>
        let reply = handle_message(msg);
        Lwt_io.write_line(oc, reply) >>= handle_connection(ic, oc);
      | None => Logs_lwt.info((m) => m("Connection closed")) >>= return
      }
  );

let accept_connection = (conn) => {
  let (fd, _) = conn;
  let ic = Lwt_io.of_fd(Lwt_io.Input, fd);
  let oc = Lwt_io.of_fd(Lwt_io.Output, fd);
  Lwt.on_failure(
    handle_connection(ic, oc, ()),
    (e) => Logs.err((m) => m("%s", Printexc.to_string(e)))
  );
  Logs_lwt.info((m) => m("New connection")) >>= return;
};

let create_socket = () => {
  open Lwt_unix;
  let sock = socket(PF_INET, SOCK_STREAM, 0);
  bind(sock) @@ [@implicit_arity] ADDR_INET(listen_address, port);
  listen(sock, backlog);
  sock;
};

let create_server = (sock) => {
  let rec serve = () => Lwt_unix.accept(sock) >>= accept_connection >>= serve;
  serve;
};

let () = {
  let () = Logs.set_reporter(Logs.format_reporter());
  let () = Logs.set_level(Some(Logs.Info));
  let sock = create_socket();
  let serve = create_server(sock);
  Lwt_main.run @@ serve();
};
