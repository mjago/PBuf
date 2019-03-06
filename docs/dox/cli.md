## Command Line Evaluation program (cli)

### Introduction

The `cli` program provides an easy way to fire up the priority buffer.

It consists of a very simple state-machine that allows the user certain instructions to manipulate the buffer.

The `cli` may be built by running `gcc cli/cli.c src/priority_buffer.c -o./cli/out && cli/out`

Alternatively a pre-built binary is available in the `cli` directory.

### Menu

The user is presented with a simple menu and may enter data, or commands:

```

	Prioritised Buffer Exerciser

priority 0 (lowest)
buffer:
 path:
 data: 0, 0, 0, 0, 0, 0, 0, 0
 next:  1, 2, 3, 4, 5, 6, 7, 0
head(0):   7, head(1):   7, head(2):   7, head(3):   7, tail:  7
active(0): 0, active(1): 0, active(2): 0, active(3): 0, empty: 1
  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .

Enter either:   1. data (a number less than 256)
  	  	        2. one of the following Commands:

  	  	        P : Priority Setting
  	        	G : Get Value
  	  	        R : Reset Buffer
  	            Q : Quit

```

### Data Display

As data is entered it appears in the buffer representation:

```

	Prioritised Buffer Exerciser

priority 0 (lowest)
buffer:
 path: 127 -> 22 -> 255
 data: 127, 22, 255, 0, 0, 0, 0, 0
 next:  1, 2, 3, 4, 5, 6, 7, 0
head(0):   2, head(1):   7, head(2):   7, head(3):   7, tail:  7
active(0): 1, active(1): 0, active(2): 0, active(3): 0, empty: 0
  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .

```
