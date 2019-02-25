# Adding Data to the Buffer

## Symbol meanings
For the sake of this document, I shall employ the following symbols in the examples that follow:
1. Priority is indicated with the letters `h`, `m`, and `l`, indicating, high, medium and low priorities. Note there
can be up to 8 levels of priority in practice, but here we will only deal with three for clarity. The number
following the letter indicates the number of elements of this priority in the buffer.

2. A number following the priority letter indicates the order in which elements are *inserted into* the buffer, rather than the order they are *retrieved from* the buffer.

2. `x` indicates unused but available storage in the buffer.

3. `H` followed by a priority letter indicate a Head related to said priority, as in `Hm` representing *Head associated with the Medium priority buffer data.*

4. `T` and `T*` represent *Tail* and *Tail pointer* respectively.

### Adding data to an empty buffer

#### Initially:

```
x  ->  x  ->  x  ->  x  ->  ...
T*
```

Since the data to be added will be the only data in the buffer it will naturally be the highest priority
data on the buffer. In this case we can simply add the data at the element indicated by the tail pointer (T*)
and set the relevant head pointer (Hl* or Hm* or Hh*). We also need to set the active flag for the relevant
priority to indicate we have data of this priority in the buffer.

### Adding l1 becomes:

```
l1  ->  x  ->  x  ->  x  ->  ...
T*
Hl*
```

## Adding data of the lowest priority to a partially full buffer containing only low priority

### Initially:

```
l1  ->  l2  ->  l3  ->  x  ->  ...
T*
                Hl      Hl*
```

Since the buffer contains data in a high priority first, through lower priorities, to lower priority order,
to add data of the lowest priority we can simply add the data at the buffer position indicated by the relevant
head pointer H* and advance the Head Hl. The relevant active flag is already set, but we set it anyway since it
is quicker to just set the flag than test the flag and set it.

### Adding l4 becomes:

```
l1  ->  l2  ->  l3  ->  l4  ->  x  ...
T*
                        Hl      Hl*
```

## Adding data of the lowest priority to a partially full buffer containing mixed priorities

### Initially:

```
h1  ->  h2  ->  h3  -> m1  ->  m2  ->  m3  ->  l1  ->  x  ->  ...
T*
                Hh                     Hm      Hl      Hl*
```

Similarly to the previous example, here we simply add the new element to the buffer at the position indicated
by the relevant head pointer. We then advance the relevant head and set the relevant active flag. Since the 
added data is of the lowest priority it needs to be at the end of the buffer, so we need to do anything more
here.

### Adding l2 becomes:

```
h1  ->  h2  ->  h3  -> m1  ->  m2  ->  m3  ->  l1  ->  l2  ->  x  ->  ...
T*
                Hh                     Hm              Hl      Hl*
```

## Adding data of a high priority to a partially full buffer containing mixed priorities

### Initially:

```
h1  ->  h2  ->  h3  -> m1  ->  m2  ->  m3  ->  l1  ->  x  ->  ...
T*
                Hh                     Hm      Hl      Hl*
```

We now wish to add high priority data to a buffer of mixed priorities. In this case we need the data to
be placed after data of a similar or higher priorities but before data of a lower priority. This is a two
stage process. Stage 1 involves where to store the data in the physical buffer and stage 2 involves
rearranging the buffer to set the data at the correct level of importance in the buffer, which is behind
than data of a higher or similar priority, but ahead of lower priority data.

### Adding h4 to the buffer:

Since the buffer is not full we can add the data to an unused element (marked by x) that is pointed to by
the lowest active Head pointer:

### Stage 1:

```
h1  ->  h2  ->  h3  -> m1  ->  m2  ->  m3  ->  l1  ->  h4  ->  x  ...
T*
                Hh                     Hm      Hl      Hl*
```

The buffer is now out of order since we have higher priority data that is behind low priority data. We
can correct this anomaly by remapping, or re-routing the buffer (since we have a link between adjacent
elements). This is achieved by reassigning links in such a way that the new entry is placed behind
data of higher and equal priority to itself, and ahead of data of lower priority to itself. This will
re-establish the correct prioritisation in the buffer.

### Finding the before insertion point:

As indicated we need to place the data behind all data of higher or equal priority. To find this point
we simply need to examine the activity flags starting at the current priority, and find the first active
flag of equal or higher priority. If no such priority is active then we become the highest priority in
the buffer and need to be inserted at the front of the buffer. If higher or equal priority exists we
establish the priority closest to the current priority in increasing priority. In our case this is the
highest priority. Therefore we need to be placed between the Hh head pointer and the element following.
This is achieved by:

1. saving the link at h3 in our case (which currently links to m1)
2. writing the position of the newly inserted data to the link at h3 (in our case the position of h4)
3. writing the current link of the newly inserted data to the pointer Hh* link (in our case m1)
4. writing the saved link to the current link (in our case h4*)
5. setting the relevant head pointer to indicate the new priority head.
6. setting the relevant priority activity flag incase it hasn't been set already.

### Stage 2:

Below shows how the links are broken and rearranged as above. The affect is to place the element h4
behind the element h3 and ahead of m1.

```
                    ------------------------------>
                      <------------------------------------
                                                    ------->
h1  ->  h2  ->  h3     m1  ->  m2  ->  m3  ->  l1      h4      x  ...
T*
```
                                       Hm      Hl      Hh

Below we see the buffer after redrawing and allowing for re-linking:

```
h1  ->  h2  ->  h3  ->  h4  ->   m1  ->  m2  ->  m3  ->  l1  ->  x  ...
T*
                        Hh                       Hm      Hl
```

## Adding data to a full buffer

To add data to a full buffer we are required to push the oldest lowest priority data 'out the back' of
the buffer, since we no longer have room to accomodate additional data. This ensures that the limited
buffer space is reserved for the higher priority data and newer data of the lowest priority.

## Attempting to add data of a lower priority that currently exists in a full buffer

If the data to be added is of a lower priority than any of the data in the buffer (indicated by an inactive
activity flag at the new priority), then the data is rejected, the buffer left untouched and an insertion
error returned to the caller.

## Adding data to a full buffer where the buffer is full of a single priority

Where we are adding newer data to a full buffer containing only data of a similar priority we can simply overwrite
the oldest data in the buffer with our newest data, and advance the tail pointer to the next position. In this
way the latest insert is pushed to the back of the queue.

### Initially:

```
l1  ->  l2  ->  l3  ->  l4  ->   l5  ->  l6  ->  l7  ->  l8  ->  l9  ->  l10
T*                                                                       T
                                                                         Hl
```


### Adding l11 becomes:

```
l11 ->  l2  ->  l3  ->  l4  ->   l5  ->  l6  ->  l7  ->  l8  ->  l9  ->  l10
T*                                                                       T
                                                                         Hl
```
### Then advancing the Tail pointer:

```
l2  ->  l3  ->  l4  ->   l5  ->  l6  ->  l7  ->  l8  ->  l9  ->  l10  ->  l11
T*                                                                        T
                                                                          Hl
```

## Adding data to a full buffer containing a mix of priority

Assuming we have data of a higher or equal priority we can add it to the buffer by pushing less important
data (older or lower priority) out of the buffer. This is a two stage process - stage 1 is involved with
determining where to store the new data - stage 2 involves remapping or re-routing the links of the buffer
to insert the data between data of higher importance (higher priority or newer) and data of lower importance
(lower priority).

### Stage 1:

#### To add m4:

```
h1  ->  h2  ->  h3  ->  h4  ->   m1  ->  m2  ->  m3  ->  l1  ->  l2
T*                                                               T
                        Hh                       Hm              Hl
```

Here we have a full buffer containing a mix of prioritisation. To add our data (m4) we need to determine
the element containing the lowest priority and oldest element. In our case this is the element l1. Regardless
of which priority we are adding this is the element to get overwritten since it it has the least importance.

### Overwriting:

```
h1  ->  h2  ->  h3  ->  h4  ->   m1  ->  m2  ->  m3  ->  m4  ->  l2
T*                                                               T
                        Hh                       Hm              Hl
```

Here we need to overwrite l1 with m4 as shown. Since we haven't a direct pointer to l1 we need to find
l1 some other way. l1 can be considered the 'tail' of priority 'l'. This can be determined by finding the
next highest priority on the buffer (that is the next active priority) and following the link of that
priority's relevant head. In our case the next highest priority to l1 is priority m. We follow the link
given by the head Hm to l1 and overwrite m4 there.

In this case the buffer is still in the correct order and we simply need to advance the Hm head:

```
h1  ->  h2  ->  h3  ->  h4  ->   m1  ->  m2  ->  m3  ->  m4  ->  l2
T*                                                               T
                        Hh                               Hm      Hl
```

## Adding data to a full buffer of a priority higher than any on the buffer

### Initially:

```
m1  ->  m2  ->  m3  ->  m4  ->  l1  ->  l2  ->  l3  ->  l4  ->   l5
T*                                                               T
                        Hm                                       Hl
```

In this buffer we have data of priority m and data of priority l but no data of priority h. Say we need
to add h1 to the buffer. We first need to identify the oldest, lowest priority element and overwrite it
with the new data:

### Overwriting l1 with h1:

```
m1  ->  m2  ->  m3  ->  m4  ->  h1  ->  l2  ->  l3  ->  l4  ->   l5  (  ->  m1)
T*                                                               T   (      T*)
                        Hm                                       Hl
```

Here we have overwritten l1 with h1. However the buffer clearly requires re-adjusting to correct the priorities.
This is a similar process to that considered above.

### Required adjustment:

```
                             <------------------------------------
<------------------------------------
                            -------->
m1  ->  m2  ->  m3  ->  m4      h1      l2  ->  l3  ->  l4  ->   l5
T*                                                               T
                        Hm                                       Hl
```

Notice here that the T* needs to move since the higher priority becomes the front of the queue;

### Rearranged gives:

```
h1  ->  m1  ->  m2  ->  m3  ->  m4  ->  l2  ->  l3  ->  l4  ->   l5
T*                                                               T
                                Hm                               Hl
```

## Retrieving Data from the buffer:

### Initially:

```
l1  ->  l2  ->  l3  ->  l4  ->   l5  ->  l6  ->  l7  ->  l8  ->  l9  ->  l10
T*                                                                       T
                                                                         Hl
```

To remove data from the buffer we first check that the buffer is not empty. If it is empty an
error is returned to the caller. If the buffer is not empty we return the element pointed to
by the Tail pointer T* and advance the Tail pointer.

### Following retrieval:

```
l2  ->  l3  ->  l4  ->   l5  ->  l6  ->  l7  ->  l8  ->  l9  ->  l10
T*                                                               T
                                                                 Hl
```

We must determine whether we have depleted a particular priority from the buffer, and mark that priority 
inactive if this is the case. This is detected by comparing the highest active priority's head pointer with
the tail pointer prior to advancing the tail pointer. If they are equal we are about to deplete the highest
priority and need to make the priority inactive:

### Depleted Priority:

```
h1  ->  l1  ->  l2  ->  l3  ->  l4  ->   l5  ->  l6  ->  l7  ->  l8
T*                                                               T
Hh                                                               Hl
```

In this case there is only a single active priority on the buffer and a single relevant head pointer:

### Becomes:

```
l1  ->  l2  ->  l3  ->  l4  ->   l5  ->  l6  ->  l7  ->  l8
T*                                                       T
                                                         Hl
```
