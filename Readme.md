## libcoroutine

A small library to implement stackful coroutines in C

## Usage

To create a coroutine, first create a `coroutine_data` struct and call either 
`coroutine_start` or `coroutine_start_with_mem` to start a new coroutine. The coroutine
will initialize but will not start running until `coroutine_next` is called. Then, the coroutine
will run until it either calls `coroutine_yield` or returns. `coroutine_continue` can be used
to pass control directly to another coroutine, and so can be used to create more exotic types 
of control flow.

## Implementation

Coroutines are implemented by using a small assembly routine to switch the stacks between 
the main stack of the program and the stack of the coroutine. It does this by saving all
registers and then swapping the stack pointer. Currently assembly has only been implemented
for x86 and x86-64 so this library currently does not work on other architectures. 

Caveats of this implementation are that it is very easy for the coroutine method to overflow
the stack buffer that it is placed in. In addition, it is very difficult (or undecidable if 
recursion is involved) to determine the stack size that will be used by any given function.
