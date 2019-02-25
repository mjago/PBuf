## Command Line Evaluation program (cli)

### Introduction

The `cli` program provides an easy way to fire up the priority buffer.

It consists of a very simple state-machine that allows the user certain instructions to manipulate the buffer.

The `cli` may be built by running `gcc cli/cli.c src/priority_buffer.c -o./cli/out && cli/out`

Alternatively a pre-built binary is available in the `cli` directory.

### Menu

The user is presented with a simple menu and the following commands are made available:

```
Enter one of the following:
    E : Enter Values
    P : Priority
    G : Get Value
    R : Reset Buffer
    S : Show Size
    Q : Quit
```
