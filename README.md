# PBuf (v0.2.0)

PBuf is a Prioritised buffer intended for use in an embedded environment,
with processors or controllers as small as 8-bit.

## Purpose

The Prioritised Buffer can be used to enable the rapid re-prioritisation of self-contained payloads of data, possibly compressed
and encrypted. It makes sense to manipulate such a payload in units of payload, and being able to prioritise or re-order the
delivery of them against time can release the business logic from being required to maintain a lot of state regarding what
has gone, what has maybe gone, and how urgent is this payload with respect to what is already out there. By employing prioritisation
in this way, the priorities may be worked out ahead of time, and the configuration stored as constant data for rapid use, freeing
the user of much of the decision-making in real-time.

## Configuration

It is configurable at compile time by defining three definitions (found in priority_buffer.h).
The definitions can also be passed to the compiler via the command line.

1. `BUFFER_SIZE` is the number of buffer elements (3 to 256).
2. `ELEMENT_SIZE` is the size of each element (8, 16, 32, or 64 bits).
3. `PRIORITY_SIZE` is the number of priorities used by the buffer (2 to 8).

The compilation will fail if other values are attempted.

By default these are set to `4`, `8` and `3` respectively.

## Test

A test suite is available in `test/` and can be run by typing `make` in the root directory.

The testing framework used is [Unity Test System](https://github.com/throwtheswitch/). The
test runners are written in C to avoid other dependencies. [Unity Test System](https://github.com/throwtheswitch/) is MIT licensed.

## Cli

A cli program is available in `cli/`.
This *buffer exerciser* is configured for an eight element buffer and four priorities, but may be adjusted (see makefile).
It enables for the *inserting*, *retrieving*, *viewing* and *resetting* of data in a *prioritised buffer*.

To build *cli* run `make build_cli`

To run *cli* run `make run_cli`

## Documentation
Some documentation is present in the `docs` directory and is available at the link below.

The source documentation is cross-referenced using [Doxygen](http://www.doxygen.nl/). and can be built using the command `make doc`.

[The documentation is available here.]( http://martynjago.co.uk/PBuf/).

## Concurrency

There are currently no locks or checks for concurrency, so it is the responsibility of the user to ensure
reads and writes do not occur simultaneously. This is by design, since the user has control over their interrupts
etc.

## Licence

PBuf has a permissive MIT license (see ./LICENSE)

## Contributors

- [mjago](https://github.com/mjago/sonicri)

