# PBuf

### (Prioritised Buffer)

### Note: Alpha Software (under development)

This software is a Priority buffer intended for use in an embedded environment,
with processors or controllers as small as 8-bit.

## Configuration

It is configurable at compile time by defining three definitions (found in priority_buffer.h).
The definitions can also be passed to the compiler via the command line.

1. `BUFFER_SIZE` is the number of buffer elements (3 to 256).
2. `ELEMENT_SIZE` is the size of each element (8, 16, 32, or 64 bits).
3. `PRIORITY_SIZE` is the number of priorities used by the buffer (2 to 8).

The compilation will fail if other values are attempted.

By default they are set to `4, 8 and 3` respectively.

## Test

A test suite is available in `test/` and can be run by typing make in this directory.

The testing framework used is [Unity Test System](https://github.com/throwtheswitch/). The
test runners are written in C to avoid other dependencies. [Unity Test System](https://github.com/throwtheswitch/) is MIT licensed.

## Cli

A cli program is available in `cli/`

This is currently built and run with the following command:

```shell
 gcc cli/cli.c src/priority_buffer.c -o./cli/pbuf && cli/pbuf
 ```
 `cli` builds with `PBuf` in its default configuration and enables *inserting, retrieving, viewing and resetting*
 a *priority buffer*.

## Documentation
Some documentation is present in the `docs` directory, although it is not yet exhaustive.

Source documentation is cross-referenced using doxygen and can be built using the command `doxygen docs/doxyfile`.

This can be accessed [here](https://github.com/mjago/pbub).

## Concurrency

There are currently no locks or checks for concurrency, so it is the responsibility of the user to ensure
reads and writes do not occur simultaneously. This is by design, since the user has control over their interrupts
etc. and this library doesn't.

## Licence

PBuf has a permissive MIT license (see ./LICENSE)

## Contributors

- [mjago](https://github.com/mjago/sonicri)

