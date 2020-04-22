(*
Create `./dune` file then run:
`dune install`
Build it by running in terminal in this dir:
 `dune build hello_world.exe`.
 Then run with `dune exec ./hello_world.exe`
  *)
Lwt_main.run (Lwt_io.printf "Hello, world!\n")