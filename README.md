# toyrobot
Toy Robot Challenge

All ofthe toy robot challenge code is in the single file `toyrobot.cpp`.
The code has been built and tested under Linux with G++ 9.4.

## building

    make

## testing

    make test

## usage

    $ ./toyrobot [command file] [output file]

The `toyrobot` binary will by default read commands from `STDIN` and
write output to `STDOUT`.

It can also optionally take an input file formatted to contain one
command per line.

If an input file is provided and output file to capture all output
can optionally be provided, otherwise all output will still be written
to `STDOUT`.
