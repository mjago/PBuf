# Theory of operation

This is a description of the theory of operation of this prioritorised circular buffer.

## Configuration

The buffer is configured at compile time by setting a small number of defines in priority_buffer.h:
```
#define BUFFER_SIZE 4      /* may be between 4 to 256 elements in size */

#define PRIORITY_SIZE 3    /* may be between 2 to 8 priorities in size */

 #define ELEMENT_SIZE 8     /* may be either 8, 16, 32, or 64 bits */
```
The compiler checks these settings at compile time and compile will fail if they are out of limits.

These settings may also be provided on the command line to the compiler such as -DELEMENT_SIZE=8

priority_buffer.h is purposefully kept very minimal to highlight these user configurations.

## Design choice

The design is optimised for smaller embedded devices, and should be suitable down to 8-bit devices.

## Requirement Assumptions

Due to the nature and size of the project it was decided to push through a design and then refactor
for any wrong assumptions in the design and implementation. The design is unit-tested to ensure this
refactoring is as painless as possible.

## Memory

The circular buffer is a structure containing elements upto the defined BUFFER_SIZE. Each element is
a structure containing both data and a forward-linking link to the following buffer location.

We also have a tail pointer and and array of head pointers whose size is equal to the PRIORITY_SIZE.
In addition, a further byte is used to store active flags for each priority.

With these pointers into the structure we can process both insert and retrieve operations very quickly.
The only time we spend following the links to any extent (beyond one or two) is when we are requred to
calculate the size of the buffer contents since it was decided the memory saved here was justifiable
(the print function also follows the links, but this is a DEBUG enabled function only - for the
purpose of the command line evaluation program).

## Pointers

As mentioned, a number of pointers are used to provide fast access into the buffer:

Here P1 refers to highest priority, P3 to lowest priority etc.
Tail is the tail pointer.
Head is the head pointer.
The active array maintains the active status.
the x denotes an empty element.
```
---------------------<------------------
|                                      |
->  P1   ->   P2   ->   P3   ->   x   >-
    ^         ^         ^         ^
 Head[1]   Head[2]   Head[3]     Tail

Active[1] = ACTIVE
Active[2] = ACTIVE
Active[3] = ACTIVE
```
## Retrieving Data

To retrieve an element the tail updates by following the link of the element it refernces, and the data at
its new position is retrieved. If the head pointer matching the retrieved element's priority has the same
index value as the tail pointer, the relevant active flag is set to NON_ACTIVE.

If there is no data in the buffer all active flags are NON_ACTIVE and a suitable message returned to the
caller. The empty buffer check is a single instruction operation.

## Adding Data

Since we prioritise the data added to the buffer the direction of priorities can be considered as flowing away
from the tail from oldest high priority through newest high priority all the way down to oldest low priority
through to newest low priority. This means that data is always retrieved highest priority first but in a first-
in-first-out manner considering the priority.

Elements are added to the buffer by the following means:

### Buffer Empty

If the buffer is empty, we add data to the location linked by the tail pointer. We ensure the data's
priority is noted on the relevant active flag, and assign the relevant priority head pointer with the
array index of the element's position in the buffer. In this way the priority head pointers always point
to the newest element having that priority. If any active flag is INACTIVE the associated head pointer's
data is contains a meaningless value.

### Buffer containing data but not full

If the buffer is neither empty nor full, we add data to the location referenced by the `virtual master head'
pointer. This is simply the index value of the lowest active priority pointer. In this case the data may be
of a higher priority than its predecesser in which case we need to move it to the correct position. This is
dealt with below by remapping the buffer.

### Buffer Full

If the buffer is full we need to determine if there is older data we can overwrite rather than throwing away
this new data. This is achieved by the following strategy:

If there is data in the buffer having a priority that is either equal or of a lesser priority then we pick the
lowest priority, and the oldest entry within that priority. We can reference this value by examining the priority
head pointers and picking the next highest active priority to the one we want and following its link. This is where
we need to overwrite our new data. It may happen that there is no higher priority active in which case our location
is linked to by the value of the tail pointer index. Once we've overwritten the older data we will need to rearrange
our links to relocate the latest entry to be at the end of it's queue of identical priority. This is described below.

## Remapping the buffer

When data of varying priorities are added to the buffer we then need to rearrange the buffer to place the new element
at the correct position in the buffer relevant to its priority. We are only concerned with remapping a single element
since this is what we added, and the operation is very simple and quick.

NOTE: This process is described more thoroughly in the `adding_data_to_the_buffer` document.
