/*
 * Concurrent Programming with Lwt
 *
 * Written in 2017 by Deokhwan Kim
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
/* The latest version of this document is available at https://github.com/dkim/rwo-lwt/. */
/* Async Basics */
open Lwt;

let file_contents = (filename: Lwt_io.file_name): Lwt.t(string) =>
  Lwt_io.with_file(~mode=Lwt_io.input, filename, channel =>
    Lwt_io.read(channel)
  );

let save = (filename: Lwt_io.file_name, ~contents: string): Lwt.t(unit) =>
  Lwt_io.with_file(~mode=Lwt_io.output, filename, channel =>
    Lwt_io.write(channel, contents)
  );

let uppercase_file = (filename: Lwt_io.file_name): Lwt.t(unit) => {
  let%lwt text = file_contents(filename);
  save(filename, ~contents=String.uppercase_ascii(text));
};

let count_lines = (filename: Lwt_io.file_name): Lwt.t(int) => {
  let%lwt text = file_contents(filename);
  String.split_on_char('\n', text) |> List.length |> Lwt.return;
};

/* Ivars and Upon */

module type Delayer_intf = {
  type t;
  let create: float => t;
  let schedule: (t, unit => Lwt.t('a)) => Lwt.t('a);
};

module Delayer: Delayer_intf = {
  type t = {
    delay: float,
    jobs: Queue.t(unit => unit),
  };

  let create = (delay: float): t => {delay, jobs: Queue.create()};

  let schedule = (t: t, thunk: unit => Lwt.t('a)): Lwt.t('a) => {
    let (waiter, wakener) = Lwt.wait();
    Queue.add(
      () =>
        Lwt.on_any(thunk(), Lwt.wakeup(wakener), Lwt.wakeup_exn(wakener)),
      t.jobs,
    );
    Lwt.on_termination(Lwt_unix.sleep(t.delay), Queue.take(t.jobs));
    waiter;
  };
};

/* Example: An Echo Server */

let rec copy_blocks =
        (buffer: bytes, r: Lwt_io.input_channel, w: Lwt_io.output_channel)
        : Lwt.t(unit) =>
  switch%lwt (Lwt_io.read_into(r, buffer, 0, Bytes.length(buffer))) {
  | 0 => Lwt.return_unit
  | bytes_read =>
    let%lwt () = Lwt_io.write_from_exactly(w, buffer, 0, bytes_read);
    copy_blocks(buffer, r, w);
  };

/*
 let run () : unit =
   ((let%lwt server =
       Lwt_io.establish_server (Lwt_unix.ADDR_INET (Unix.inet_addr_any, 8765))
         (fun (r, w) ->
            let buffer = Bytes.create (16 * 1024) in
            copy_blocks buffer r w)
     in
     Lwt.return server) : Lwt_io.server Lwt.t) |> ignore
 */

let never_terminate: 'a. Lwt.t('a) = fst(Lwt.wait());

/*
 let () =
   Sys.set_signal Sys.sigpipe Sys.Signal_ignore;
   (try Lwt_engine.set (new Lwt_engine.libev ())
    with Lwt_sys.Not_available _ -> ());
   run ();
   Lwt_main.run never_terminate
 */

/* Improving the Echo Server */

let run = (uppercase: bool, port: int): Lwt.t(unit) => {
  let%lwt server =
    Lwt_io.establish_server(
      [@implicit_arity] Lwt_unix.ADDR_INET(Unix.inet_addr_any, port),
      ((r, w)) =>
      Lwt_io.read_chars(r)
      |> (
        if (uppercase) {
          Lwt_stream.map(Char.uppercase_ascii);
        } else {
          x => x;
        }
      )
      |> Lwt_io.write_chars(w)
    );

  (server: Lwt_io.server) |> ignore;
  never_terminate;
};

/*
 let run (uppercase : bool) (port : int) : unit Lwt.t =
   let%lwt server =
     Lwt_io.establish_server (Lwt_unix.ADDR_INET (Unix.inet_addr_any, port))
       (fun (r, w) ->
          let reader = Lwt_pipe.IO.read r in
          let writer =
            Lwt_pipe.IO.write w
            |> (if uppercase then Lwt_pipe.Writer.map ~f:String.uppercase_ascii
                else fun x -> x)
          in
          Lwt_pipe.connect ~ownership:`OutOwnsIn reader writer;
          Lwt_pipe.wait writer)
   in
   (server : Lwt_io.server) |> ignore;
   never_terminate
 */

/*
 let () =
   let uppercase = ref false
   and port = ref 8765 in
   let options =
     Arg.align [
       ("-uppercase",
        Arg.Set uppercase,
        " Convert to uppercase before echoing back");
       ("-port",
        Arg.Set_int port,
        "num Port to listen on (default 8765)");
     ]
   in
   let usage = "Usage: " ^ Sys.executable_name ^ " [-uppercase] [-port num]" in
   Arg.parse
     options
     (fun arg -> raise (Arg.Bad (Printf.sprintf "invalid argument -- '%s'" arg)))
     usage;

   Sys.set_signal Sys.sigpipe Sys.Signal_ignore;
   (try Lwt_engine.set (new Lwt_engine.libev ())
    with Lwt_sys.Not_available _ -> ());
   Lwt_main.run (run !uppercase !port)
 */

/* Example: Searching Definitions with DuckDuckGo */

/* URI Handling */

/*
 let query_uri : string -> Uri.t =
   let base_uri = Uri.of_string "https://api.duckduckgo.com/?format=json" in
   (fun query -> Uri.add_query_param base_uri ("q", [query]))
 */

/* Parsing JSON Strings */

let get_definition_from_json = (json: string): option(string) =>
  switch (Yojson.Safe.from_string(json)) {
  | `Assoc(kv_list) =>
    let find = key =>
      switch (List.assoc(key, kv_list)) {
      | exception Not_found => None
      | `String("") => None
      | s => Some(Yojson.Safe.to_string(s))
      };

    switch (find("Abstract")) {
    | Some(_) as x => x
    | None => find("Definition")
    };
  | _ => None
  };

/* Executing an HTTP Client Query */

/*
 let get_definition (word : string) : (string * string option) Lwt.t =
   let%lwt _resp, body = Cohttp_lwt_unix.Client.get (query_uri word) in
   let%lwt body' = Cohttp_lwt_body.to_string body in
   Lwt.return (word, get_definition_from_json body')

 let print_result ((word, definition) : string * string option) : unit Lwt.t =
   Lwt_io.printf "%s\n%s\n\n%s\n\n"
     word
     (String.init (String.length word) (fun _ -> '-'))
     (match definition with
      | None -> "No definition found"
      | Some def ->
        Format.pp_set_margin Format.str_formatter 70;
        Format.pp_print_text Format.str_formatter def;
        Format.flush_str_formatter ())

 let search_and_print (words : string list) : unit Lwt.t =
   let%lwt results = Lwt_list.map_p get_definition words in
   Lwt_list.iter_s print_result results

 (*
 let search_and_print (words : string list) : unit Lwt.t =
   Lwt_list.iter_p
     (fun word ->
        let%lwt result = get_definition word in
        print_result result)
     words
 *)

 let () =
   let words = ref [] in
   let usage = "Usage: " ^ Sys.executable_name ^ " [word ...]" in
   Arg.parse [] (fun w -> words := w :: !words) usage;
   words := List.rev !words;

   (try Lwt_engine.set (new Lwt_engine.libev ())
    with Lwt_sys.Not_available _ -> ());
   Lwt_main.run (search_and_print !words)
 */

/* Example: Handling Exceptions with DuckDuckGo */

let query_uri = (~server: string, query: string): Uri.t => {
  let base_uri =
    Uri.of_string(String.concat("", ["https://", server, "/?format=json"]));

  Uri.add_query_param(base_uri, ("q", [query]));
};

/*
 let get_definition ~(server : string) (word : string) : (string * (string option, string) result) Lwt.t =
   try%lwt
     let%lwt _resp, body = Cohttp_lwt_unix.Client.get (query_uri ~server word) in
     let%lwt body' = Cohttp_lwt_body.to_string body in
     Lwt.return (word, Ok (get_definition_from_json body'))
   with _ -> Lwt.return (word, Error "Unexpected failure")
 */

let print_result =
    ((word, definition): (string, result(option(string), string)))
    : Lwt.t(unit) =>
  Lwt_io.printf(
    "%s\n%s\n\n%s\n\n",
    word,
    String.init(String.length(word), _ => '-'),
    switch (definition) {
    | Error(s) => "DuckDuckGo query failed: " ++ s
    | Ok(None) => "No definition found"
    | Ok(Some(def)) =>
      Format.pp_set_margin(Format.str_formatter, 70);
      Format.pp_print_text(Format.str_formatter, def);
      Format.flush_str_formatter();
    },
  );

/*
 let search_and_print ~(servers : string list) (words : string list) : unit Lwt.t =
   let servers = Array.of_list servers in
   let%lwt results =
     Lwt_list.mapi_p
       (fun i word ->
          let server = servers.(i mod Array.length servers) in
          get_definition ~server word)
       words
   in
   Lwt_list.iter_s print_result results

 let () =
   let servers = ref ["api.duckduckgo.com"]
   and words = ref [] in
   let options =
     Arg.align [
       ("-servers",
        Arg.String (fun s -> servers := String.split_on_char ',' s),
        "s1,...,sn Specify servers to connect to");
     ]
   in
   let usage = "Usage: " ^ Sys.executable_name ^ " [-servers s1,...,sn] [word ...]" in
   Arg.parse options (fun w -> words := w :: !words) usage;
   words := List.rev !words;

   (try Lwt_engine.set (new Lwt_engine.libev ())
    with Lwt_sys.Not_available _ -> ());
   Lwt_main.run (search_and_print ~servers:!servers !words)
 */

/* Timeouts, Cancellation, and Choices */

let get_definition =
    (~server: string, word: string)
    : Lwt.t((string, result(option(string), exn))) =>
  try%lwt(
    {
      let%lwt (_resp, body) =
        Cohttp_lwt_unix.Client.get(query_uri(~server, word));
      let%lwt body' = Cohttp_lwt_body.to_string(body);
      Lwt.return((word, Ok(get_definition_from_json(body'))));
    }
  ) {
  | exn => Lwt.return((word, Error(exn)))
  };

let get_definition_with_timeout =
    (~server: string, timeout: float, word: string)
    : Lwt.t((string, result(option(string), string))) =>
  Lwt.pick([
    {
      let%lwt () = Lwt_unix.sleep(timeout);
      Lwt.return((word, Error("Timed out")));
    },
    {
      let%lwt (word, result) = get_definition(~server, word);
      let result' =
        switch (result) {
        | Ok(_) as x => x
        | Error(_) => Error("Unexpected failure")
        };

      Lwt.return((word, result'));
    },
  ]);

let search_and_print =
    (~servers: list(string), timeout: float, words: list(string))
    : Lwt.t(unit) => {
  let servers = Array.of_list(servers);
  let%lwt results =
    Lwt_list.mapi_p(
      (i, word) => {
        let server = servers[i mod Array.length(servers)];
        get_definition_with_timeout(~server, timeout, word);
      },
      words,
    );

  Lwt_list.iter_s(print_result, results);
};

/*
 let () =
   let servers = ref ["api.duckduckgo.com"]
   and timeout = ref 5.0
   and words = ref [] in
   let options =
     Arg.align [
       ("-servers",
        Arg.String (fun s -> servers := String.split_on_char ',' s),
        "s1,...,sn Specify servers to connect to");
       ("-timeout",
        Arg.Set_float timeout,
        "secs Abandon queries that take longer than this time");
     ]
   in
   let usage = "Usage: " ^ Sys.executable_name ^ " [-servers s1,...,sn] [-timeout secs] [word ...]" in
   Arg.parse options (fun w -> words := w :: !words) usage;
   words := List.rev !words;

   (try Lwt_engine.set (new Lwt_engine.libev ())
    with Lwt_sys.Not_available _ -> ());
   Lwt_main.run (search_and_print ~servers:!servers !timeout !words)
 */

/* Working with System Threads */

let rec every =
        (
          ~stop: Lwt.t(unit)=never_terminate,
          span: float,
          f: unit => Lwt.t(unit),
        )
        : Lwt.t(unit) =>
  if (Lwt.is_sleeping(stop)) {
    let%lwt () = f();
    let%lwt () = Lwt.pick([Lwt_unix.sleep(span), Lwt.protected(stop)]);
    every(~stop, span, f);
  } else {
    Lwt.return_unit;
  };

let log_delays = (thunk: unit => Lwt.t(unit)): Lwt.t(unit) => {
  let start = Unix.gettimeofday();
  let print_time = () => {
    let diff = Unix.gettimeofday() -. start;
    Lwt_io.printf("%f, ", diff);
  };

  let d = thunk();
  let%lwt () = every(0.1, ~stop=d, print_time);
  let%lwt () = d;
  let%lwt () = print_time();
  Lwt_io.print("\n");
};

let noalloc_busy_loop = (): unit =>
  for (_i in 0 to 10_000_000_000) {
    ();
  };

let () =
  Lwt_main.run @@
  log_delays(() => Lwt_preemptive.detach(noalloc_busy_loop, ()));
