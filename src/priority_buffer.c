#include <stdio.h>
#include <inttypes.h>
#include "priority_buffer.h"
#include "defs.h"

//////////////////////////////// index ////////////////////////////////

STATIC check_t checkIndex(index_t index);
STATIC check_t nextIndex(index_t * index, priority_t priority);
STATIC check_t writeNextIndex(index_t current, index_t next);
STATIC check_t incTail(void);
STATIC check_t nextTailIndex(index_t * index);
STATIC check_t firstFreeElementIndex(index_t * index);
STATIC check_t remapNotFull(index_t newIndex, priority_t priority);
STATIC check_t remap(index_t a1, index_t a2, index_t b);
STATIC index_t headValue(priority_t priority);
STATIC index_t tailIndex(void);
STATIC index_t writeTail(index_t index);
STATIC index_t lowestPriorityTail(void);
STATIC index_t insertPoint(priority_t priority);

//////////////////////////////// priority ////////////////////////////////

STATIC void resetBufferPointers(void);
STATIC void resetBuffer(void);
STATIC index_t nextHeadValue(priority_t priority);
STATIC check_t setHead(index_t index, priority_t priority);
STATIC check_t lowestPriority(priority_t * priority);
STATIC check_t validatePriority(priority_t priority);
STATIC check_t activeStatus(priority_t priority);
STATIC check_t highestPriority(priority_t * priority);
STATIC check_t setActive(priority_t priority);
STATIC check_t setInactive(priority_t priority);
STATIC check_t nextHighestPriority(priority_t * nextPriority, priority_t priority);
STATIC uint8_t activePriorityCount(void);

//////////////////////////////// element ////////////////////////////////

STATIC check_t writeElement(element_t element, priority_t priority);
STATIC check_t overwriteElement(element_t element, priority_t newPriority);
STATIC check_t readElement(element_t * element);
STATIC check_t readNextElement(element_t * element);
STATIC check_t insert(element_t element, priority_t priority);
STATIC check_t writeData(element_t element, index_t index);
STATIC check_t bufferFull(void);
STATIC check_t bufferEmpty(void);
STATIC check_t insertEmpty(element_t element, priority_t newPriority);
STATIC check_t insertNotFull(element_t element, priority_t newPriority);
STATIC check_t insertFull(element_t element, priority_t newPriority);

  /**
     Array of buffer composite elements */

STATIC pbuf_t bf;

//////////////////////////////// index ////////////////////////////////

/**
   @defgroup Internal Internal
   Internal Static Functionality
   @{ */

/**
   Check the index is a valid Index
   \return VALID_INDEX or INVALID_INDEX */

STATIC check_t checkIndex(index_t index)
{
  check_t returnVal = INVALID_INDEX;

  if(index < BUFFER_SIZE)
    {
      returnVal = VALID_INDEX;
    }

  return returnVal;
}

/**
   Advance the tail to its next position in the buffer
   \return VALID_INDEX or INVALID_INDEX */

STATIC check_t incTail(void)
{
  check_t returnVal = INVALID_INDEX;
  index_t index;
  if(nextTailIndex(&index) == VALID_INDEX)
    {
      bf.ptr.tail = index;
      returnVal = VALID_INDEX;
    }
  nextTailIndex(&index);

  return returnVal;
}

/**
   Returns the index value referenced by the tail
   \return tail index */

STATIC index_t tailIndex(void)
{
  return bf.ptr.tail;
}

/**
   Write a new index to the tail pointer
   \return VALID_INDEX or INVALID_INDEX */


STATIC index_t writeTail(index_t index)
{
  check_t returnVal = checkIndex(index);

  if(returnVal == VALID_INDEX)
    {
      bf.ptr.tail = index;
    }

  return returnVal;
}

/**
   Update the passed in index with the index pointed to
   by the tail pointer after advancing it. Returns index
   validity.
   \return VALID_INDEX or INVALID_INDEX */

STATIC check_t nextTailIndex(index_t * index)
{
  *index = bf.element[tailIndex()].next;
  return VALID_INDEX;
}

/**
   Update the passed in index with the next index linked
   from the current index passed in. Returns index validity.
   \return VALID_INDEX or INVALID_INDEX */

STATIC check_t nextIndex(index_t * next, index_t current)
{
  check_t returnVal = INVALID_INDEX;

  if(current < BUFFER_SIZE)
    {
      *next = bf.element[current].next;
      returnVal = VALID_INDEX;
    }

  return returnVal;
}

/**
   Writes the index passed in to the element referenced by
   the current index passed in.
   \return VALID_INDEX or INVALID_INDEX */


STATIC check_t writeNextIndex(index_t current, index_t next)
{
  check_t returnVal = INVALID_INDEX;

  if((current < BUFFER_SIZE) &&
     (next < BUFFER_SIZE))
    {
      bf.element[current].next = next;
      returnVal = VALID_INDEX;
    }

  return returnVal;
}

/**
   find the index at which we can store.
   This is only valid when the buffer is not full.
   Check for buffer empty case first since this is a very fast test.
   \return VALID_INDEX or INVALID_INDEX */

STATIC check_t firstFreeElementIndex(index_t * index)
{
  check_t returnVal = INVALID_INDEX;
  priority_t priority;

  if((bufferEmpty() == BUFFER_EMPTY) &&
     (nextTailIndex(index) == VALID_INDEX))
    {
      returnVal = VALID_INDEX;
    }
  else if(bufferFull() == BUFFER_NOT_FULL)
    // buffer not empty - look for lowest active priority pointer
    {
      if(lowestPriority(&priority) == VALID_PRIORITY)
        {
          *index = nextHeadValue(priority);
          returnVal = VALID_INDEX;
        }
    }

  return returnVal;
}

//////////////////////////////// priority ////////////////////////////////

/**
   Return the index pointed to by the head related with the priority passed in.
   \return the index referenced by the relevant priority head */

STATIC index_t headValue(priority_t priority)
{
  return bf.ptr.head[priority];
}

/**
   Return the index pointed to by the head related to the priority passed in
   after following the head link.
   \return the index linked to by the relevant priority head */

STATIC index_t nextHeadValue(priority_t priority)
{
  return (bf.element[headValue(priority)].next);
}

/**
   Set the value of the head pointer associated with the priority passed
   in with the index passed in.
   \return VALID_HEAD or INVALID_HEAD */

STATIC check_t setHead(index_t index, priority_t priority)
{
  check_t returnVal = INVALID_HEAD;

  if((validatePriority(priority) == VALID_PRIORITY) &&
     (index < BUFFER_SIZE))
    {
      bf.ptr.head[priority] = index;
      returnVal = VALID_HEAD;
    }

  return returnVal;
}

/**
   Determine the lowest priority in the buffer.
   \return VALID_PRIORITY or INVALID_PRIORITY */

STATIC check_t lowestPriority(priority_t * priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t priCount;

  if(bufferEmpty() == BUFFER_NOT_EMPTY)
    {
      for(priCount = LOW_PRI ;priCount < PRIORITY_SIZE ;priCount++)
        {
          if(activeStatus(priCount) == ACTIVE)
            {
              *priority = priCount;
              returnVal = VALID_PRIORITY;
              break;
            }
        }
    }

  return returnVal;
}

/**
   Check the priority is a valid one.
   \return VALID_PRIORITY or INVALID_PRIORITY */

STATIC check_t validatePriority(priority_t priority)
{
  check_t returnVal = INVALID_PRIORITY;

  if(priority < PRIORITY_SIZE)
    {
      returnVal = VALID_PRIORITY;
    }

  return returnVal;
}

/**
   Return the active status of the priority passed in.
   If the priority is active ACTIVE is returned.
   \return ACTIVE or INACTIVE */

STATIC check_t activeStatus(priority_t priority)
{
  check_t returnVal = INACTIVE;

  if(validatePriority(priority) == VALID_PRIORITY)
    {
      if((bf.activity) & (1 << priority))
        {
          returnVal = ACTIVE;
        }
    }

  return returnVal;
}

/**
   Set the relevant activity flag of the priority passed in.
   \param[in] priority to set
   \return VALID_ACTIVE or INVALID_ACTIVE */

STATIC check_t setActive(priority_t priority)
{
  check_t returnVal = INVALID_ACTIVE;

  if(validatePriority(priority) == VALID_PRIORITY)
    {
      bf.activity |= (1 << priority);

      return VALID_ACTIVE;
    }

  return returnVal;
}

/**
   Reset the relevant activity flag of the priority passed in.
   \param[in] priority to reset
   \return VALID_ACTIVE or INVALID_ACTIVE */

STATIC check_t setInactive(priority_t priority)
{
  check_t returnVal = INVALID_ACTIVE;

  if(validatePriority(priority) == VALID_PRIORITY)
    {
      bf.activity &= (0xFFu ^ (1 << priority));

      return VALID_ACTIVE;
    }

  return returnVal;
}

/**
   Determine the highest priority and assign to the priority
   pointer passed in.
   Returns VALID_PRIORITY or INVALID_PRIORITY */

STATIC check_t highestPriority(priority_t * priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t priCount;

  for(priCount = HIGH_PRI ; ; priCount--)
    {
      if(activeStatus(priCount) == ACTIVE)
        {
          *priority = priCount;
          returnVal = VALID_PRIORITY;
          break;
        }
      if(priCount == LOW_PRI)
        {
          break;
        }
    }

  return returnVal;
}

/**
   Reset Heads and Tail
*/

STATIC void resetBufferPointers(void)
{
  check_t count;
  bf.ptr.tail = BUFFER_SIZE - 1;

  for(count = LOW_PRI; count < PRIORITY_SIZE; count++)
    {
      bf.ptr.head[count] = BUFFER_SIZE - 1u;
    }
}

/**
   Reset the Buffer
*/

STATIC void resetBuffer(void)
{
  uint16_t count;

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      bf.element[count].data = 0u;
      bf.element[count].next = (count + 1u) % BUFFER_SIZE;
    }

  for(count = LOW_PRI; count < PRIORITY_SIZE; count++)
    {
      setInactive(count);
    }
}

//////////////////////////////// element ////////////////////////////////

/**
   Write an element to the buffer at the next empty position in the
   linked buffer and adjust the relevant priority head.
   \return VALID_WRITE or INVALID_WRITE */

STATIC check_t writeElement(element_t element, priority_t priority)
{
  check_t returnVal = INVALID_WRITE;
  index_t index;
  priority_t virtualHead;

  if((validatePriority(priority) == VALID_PRIORITY) &&
     (bufferFull() == BUFFER_NOT_FULL))
    {
      if(lowestPriority(&virtualHead) != VALID_PRIORITY)
        {
          nextTailIndex(&index);
        }
      else
        {
          nextIndex(&index, headValue(virtualHead));
        }

      if(writeData(element, index) == VALID_INDEX)
        {
          if((setHead(index, priority)) &&
             (setActive(priority) == VALID_ACTIVE))
            {
              returnVal = VALID_WRITE;
            }
        }
    }

  return returnVal;
}

/**
   Overwrite without remapping if buffer is full and only
   a single priority exists on the buffer
   \return VALID_WRITE ro INVALID_WRITE */

STATIC check_t overwriteSinglePriority(element_t element, priority_t newPriority)
{
  check_t returnVal = INVALID_WRITE;
  index_t nextTailIdx;

  nextTailIndex(&nextTailIdx);
  writeData(element, nextTailIdx);
  setHead(nextTailIdx, newPriority);
  if(activeStatus(newPriority) == ACTIVE)
    {
      incTail();
    }
  else
    {
      setActive(newPriority);
    }

  returnVal = VALID_WRITE;

  return returnVal;
}

/**
   Overwrite element since buffer is full and there are no unused elements.
   \return VALID_WRITE or INVALID_WRITE */

STATIC check_t overwriteElement(element_t element, priority_t newPriority)
{
  check_t returnVal = INVALID_WRITE;
  index_t tailIdx;
  index_t nextHighestHeadIdx;
  index_t lowestPriTailIdx = lowestPriorityTail();
  index_t insertPt = insertPoint(newPriority);
  priority_t priorityCount = activePriorityCount();
  priority_t highestPri;
  priority_t nextHighestPri;
  priority_t lowPri;

  nextTailIndex(&tailIdx);

  // is buffer currently full of single priority?
  if(priorityCount == 1)
    {
      returnVal = overwriteSinglePriority(element,newPriority);
    }
  else
    {
      highestPriority(&highestPri);
      writeData(element, lowestPriTailIdx);
      setHead(lowestPriTailIdx, newPriority);
      lowestPriority(&lowPri);
      nextHighestPriority(&nextHighestPri, lowPri);
      nextHighestHeadIdx = headValue(nextHighestPri);
      setActive(newPriority);
      if(lowestPriTailIdx == tailIndex())
        {
          setInactive(lowPri);
        }
      if(newPriority <= highestPri)
        {
          remap(insertPt, nextHighestHeadIdx, lowestPriTailIdx);
          writeTail(headValue(lowPri));
        }
      else
        {
          remap(insertPt, nextHighestHeadIdx, lowestPriTailIdx);
        }

      returnVal = VALID_WRITE;
    }

  return returnVal;
}

/**
   Find the next highest priority to the priority we wish to insert - used to find
   where to store the new priority
   \return VALID_PRIORITY or INVALID_PRIORITY */

check_t nextHighestPriority(priority_t * nextPriority, priority_t priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t priCount;

  for(priCount = priority + 1u; priCount < PRIORITY_SIZE; priCount++)
    {
      if(activeStatus(priCount) == ACTIVE)
        {
          *nextPriority = priCount;
          returnVal = VALID_PRIORITY;
          break;
        }
    }

  return returnVal;
}

/**
   Counts the number of active priorities
   \return number of active priorities */

STATIC uint8_t activePriorityCount(void)
{
  uint8_t returnVal = 0;
  priority_t priority;

  for(priority = LOW_PRI; priority < PRIORITY_SIZE; priority++){
    {
      if(activeStatus(priority) == ACTIVE)
        {
          returnVal++;
        }
    }
  }

  return returnVal;
}

/**
   Determines the lowest priority tail index
   \return index of the lowest priority tail */

STATIC index_t lowestPriorityTail(void)
{
  priority_t lowestButOnePri;
  priority_t lowPri;
  index_t lowestTail;

  lowestPriority(&lowPri);
  nextHighestPriority(&lowestButOnePri, lowPri);
  nextIndex(&lowestTail, headValue(lowestButOnePri));
  return lowestTail;
}

/**
   Read the next element and assign to the element pointer passed in.
   Checks that there is data to read. Returns VALID_ELEMENT on success.
   \return VALID_ELEMENT or INVALID_ELEMENT */

STATIC check_t readNextElement(element_t * element)
{
  check_t returnVal = INVALID_ELEMENT;

  if(bufferEmpty() != BUFFER_EMPTY)
    {
      if(readElement(element) == VALID_ELEMENT)
        {
          returnVal = VALID_ELEMENT;
        }
    }

  return returnVal;
}

/**
   Insert an element into the buffer of a given priority and
   adjust the buffer to correct the prioritisation if required.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t insert(element_t element, priority_t newPriority)
{
  check_t returnVal = INVALID_INSERT;
  priority_t virtualHead;

  if(bufferEmpty() == BUFFER_EMPTY)
    {
      if(insertEmpty(element, newPriority) == VALID_INSERT)
        {
          returnVal = VALID_INSERT;
        }
    }

  else if(bufferFull() == BUFFER_FULL)
    {
      returnVal = insertFull(element, newPriority);
    }
  else
    {
      if((lowestPriority(&virtualHead) == VALID_PRIORITY) &&
         (newPriority > virtualHead))
        {
          insertNotFull(element, newPriority);
        }
      else
        {
          writeElement(element, newPriority);
        }
      returnVal = VALID_INSERT;
    }
  return returnVal;
}

/**
   Write the element passed in to the index passed in.
   Check the index is within the bounds of the buffer.

   \param[in] element Element to be written
   \param[in] index Index in the buffer to write to
   \return VALID_INDEX or INVALID_INDEX */

STATIC check_t writeData(element_t element, index_t index)
{
  check_t returnVal = INVALID_INDEX;
  if(index < BUFFER_SIZE)
    {
      bf.element[index].data = element;
      returnVal = VALID_INDEX;
    }

  return returnVal;
}

/**
   Buffer Full checks whether there is any room left in the buffer for a new insertion.
   A full buffer is indicated by an active priority buffer having the same value as the
   tail pointer since the tail buffer indicates the final possible element of the buffer.
   \return BUFFER_FULL or BUFFER_NOT_FULL */

STATIC check_t bufferFull(void)
{
  check_t returnVal = BUFFER_NOT_FULL;
  priority_t priority;

  for(priority = LOW_PRI ; priority < PRIORITY_SIZE; priority++)
    {
      if((activeStatus(priority) == ACTIVE) &&
         (tailIndex() == headValue(priority)))
        {
          returnVal = BUFFER_FULL;
          break;
        }
    }

  return returnVal;
}

/**
   Check whether the buffer is empty.
   \return BUFFER_EMPTY or BUFFER_NOT_EMPTY */

STATIC check_t bufferEmpty(void)
{
  check_t returnVal = BUFFER_NOT_EMPTY;

  if(!bf.activity)
    {
      returnVal = BUFFER_EMPTY;
    }

  return returnVal;
}

/**
   Insert the passed in element of the passed in priority to an empty
   buffer.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t insertEmpty(element_t element, priority_t newPriority)
{
  check_t returnVal = INVALID_INSERT;

  if(writeElement(element, newPriority) == VALID_WRITE)
    {
      returnVal = VALID_INSERT;
    }

  return returnVal;
}

/**
   Insert into a not empty not full buffer the passed in element
   of the passed in priority. Remaps the buffer to correct the
   prioritisation.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t insertNotFull(element_t element, priority_t newPriority)
{
  check_t returnVal = INVALID_INSERT;
  index_t freeIndex;

  if(firstFreeElementIndex(&freeIndex) == VALID_INDEX)
    {
      writeData(element, freeIndex);
      remapNotFull(freeIndex, newPriority);

      returnVal = VALID_INSERT;
    }

  return returnVal;
}

/**
   Insert into a full buffer the passed in element
   of the passed in priority. Remaps the buffer to correct the
   prioritisation.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t insertFull(element_t element, priority_t priority)
{
  check_t returnVal = INVALID_INSERT;
  priority_t lowPri;

  // if lower or equal priority data in buffer find lowest
  if((lowestPriority(&lowPri)) == VALID_PRIORITY)
    {
      if(lowPri <= priority)
        {
          // overwrite oldest element at lowest priority
          overwriteElement(element, priority);

          returnVal = VALID_INSERT;
        }
    }

  return returnVal;
}

/**
   Adjust the tail, and make any depleted priorities inactive.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t adjustHeads(void)
{
  check_t returnVal = VALID_HEAD;
  priority_t priority;
  index_t index;

  for(priority = HIGH_PRI ; ; priority--)
    {
      if(activeStatus(priority) == ACTIVE)
        {
          if(nextTailIndex(&index) == VALID_INDEX)
            {
              if(headValue(priority) == index)
                {
                  setInactive(priority);
                  returnVal = VALID_HEAD;
                }
            }
          break;
        }
      if(priority == LOW_PRI)
        {
          break;
        }
    }

  return returnVal;
}

/**
   Read element from the buffer and assign to the
   element pointer passed in.
   \return VALID_ELEMENT or INVALID_ELEMENT */

STATIC check_t readElement(element_t * element)
{
  check_t returnVal = INVALID_ELEMENT;
  check_t tailIdx;

  if(nextTailIndex(&tailIdx) == VALID_INDEX)
    {
      *element = bf.element[tailIdx].data;
      if((adjustHeads() == VALID_HEAD))
        {
          if(writeTail(tailIdx) == VALID_INDEX)
            {
              returnVal = VALID_ELEMENT;
            }
          //          incTail();
        }
      returnVal = VALID_ELEMENT;
    }

  return returnVal;
}

/**
   Remap the links of the indexes passed in.
   These refer to buffer elements and cause the buffer
   to be remapped or re-routed.
   We modify three links as follows:

   ```
   save a1*
   a1* = b
   a2* = b*
   b* = saved a1*
   ```

   \return VALID_REMAP or INVALID_REMAP */


STATIC check_t remap(index_t a1, index_t a2, index_t b)
{
  check_t returnVal = INVALID_REMAP;
  index_t a1ptr;
  index_t a2ptr;
  index_t bptr;

  if((checkIndex(a1) == VALID_INDEX) &&
     (checkIndex(a2) == VALID_INDEX) &&
     (checkIndex(b) == VALID_INDEX))
    {
      nextIndex(&a1ptr, a1);
      nextIndex(&bptr, b);
      nextIndex(&a2ptr, a2);
      if(a1ptr != b)// && (a2ptr != bptr))
        {
          writeNextIndex(a1, b);
          nextIndex(&bptr, b);
          writeNextIndex(a2, bptr);
          writeNextIndex(b, a1ptr);
          returnVal = VALID_REMAP;
        }
    }

  return returnVal;
}

/**
   Return the index of the valid insert point to be used when remapping the buffer. The priority
   passed in is the priority of the newly added element. See the 'Adding data to the Buffer' document
   for more information.
   \return VALID_PRIORITY or INVALID_PRIORITY */

index_t insertPoint(priority_t priority)
{
  index_t returnVal = 255;
  index_t higherPriHeadIdx;
  index_t higherPriHeadIdxNext;
  priority_t lowestPri;
  priority_t higherPri;
  priority_t highestPri;

  if(highestPriority(&highestPri) == VALID_PRIORITY)
    {
      // priority higher to highest pri in use?
      if(priority > highestPri)
        {
          returnVal = tailIndex();
        }
      else if(bufferFull() == BUFFER_NOT_FULL)
        {
          {
            returnVal = headValue(highestPri);
          }
        }
      else if((highestPriority(&highestPri) == VALID_PRIORITY) &&
              (lowestPriority(&lowestPri) == VALID_PRIORITY) &&
              (nextHighestPriority(&higherPri, lowestPri) == VALID_PRIORITY))
        {
          higherPriHeadIdx = headValue(higherPri);
          if(nextIndex(&higherPriHeadIdxNext, higherPriHeadIdx) == VALID_INDEX)
            {
              if(higherPriHeadIdxNext == tailIndex())
                {
                  returnVal = headValue(higherPri);
                }
              else
                {
                  returnVal = headValue(priority);
                }
            }
        }
    }

  return returnVal;
}

/**
   Remap buffer for a not full buffer for the index of priority
   passed in.
   \return VALID_PRIORITY or INVALID_PRIORITY */

STATIC check_t remapNotFull(index_t newIndex, priority_t priority)
{
  check_t returnVal = INVALID_REMAP;
  index_t tailIdx;
  index_t insertionPt;
  index_t a2;
  priority_t virtualPriority;

  insertionPt = insertPoint(priority);

  if(lowestPriority(&virtualPriority) == VALID_PRIORITY)
    {
      a2 = headValue(virtualPriority);

      if(setHead(newIndex, priority) == VALID_HEAD)
        {
          if((setActive(priority) == VALID_ACTIVE) &&
             (nextTailIndex(&tailIdx) == VALID_INDEX))
            {
              if(tailIndex() == newIndex)
                {
                  bf.ptr.tail = a2;
                }
            }
        }
      remap(insertionPt, a2, newIndex);
      returnVal = VALID_REMAP;
    }

  return returnVal;
}

/** @} */
/* end of Internal group */

//////////////////////////////// Exposed API ////////////////////////////////

/**
   @defgroup API API
   This is the exposed API
   @{ */

/**
   Reset Buffer */

void PBUF_reset(void)
{
  resetBufferPointers();
  resetBuffer();
}

/**
   Return non-zero if buffer is empty.
*/

int PBUF_empty(void)
{
  return (bufferEmpty() == BUFFER_EMPTY);
}

/**
   Return non-zero if the buffer is full.
*/

int PBUF_full(void)
{
  return bufferFull() == BUFFER_FULL;
}

/**
   Return the size of the buffer.
*/

int PBUF_bufferSize(void)
{
  return BUFFER_SIZE;
}

/**
   Insert data into the buffer of the given priority.
   \return zero for a valid insert.
   \return non-zero for an invalid insert. */

int PBUF_insert(element_t element, priority_t priority)
{
  return !(insert(element, priority) == VALID_INSERT);
}

/**
   Retrieve an element from the tail of the buffer and assign to
   the element pointer passed in.
   \return zero on successful retrieve.
   \return non-zero on failed retrieve. */

int PBUF_retrieve(element_t * element)
{
  check_t returnVal = INVALID_RETRIEVE;

  if( ! PBUF_empty())
    {
      returnVal = !(readNextElement(element) == VALID_ELEMENT);
    }
  return returnVal;
}

/** @} */
/* end of API group */

/**
   Print debug output of the buffer and pointer contents.
   Requires DEBUG to be defined at compile time.
*/

//#ifdef DEBUG

void PBUF_print(void)
{
  uint16_t count;
  index_t index;
  index_t lastIndex;
  priority_t vmh;

  printf("buffer:\n path: ");

  if(bufferEmpty() == BUFFER_NOT_EMPTY)
    {
      index = tailIndex();
      lowestPriority(&vmh);
      lastIndex = headValue(vmh);
      count = 0;
      do
        {
          if(count > 0)
            {
              printf(" -> ");
            }
          nextIndex(&index, index);
          printf("%u", bf.element[index].data);
          count++;
          //          } while(count < 4);
        } while(index != lastIndex);
    }

  printf("\n data: ");

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      if(count > 0)
        {
          printf(", ");
        }
      printf("%u", bf.element[count].data);
    }
  printf("\n next:  ");

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      if(count > 0)
        {
          printf(", ");
        }
      printf("%u", bf.element[count].next);
    }
  printf("\nhead[low]: %u,\thead[mid]: %u,\thead[high]: %u,\ttail: %u,",
         headValue(LOW_PRI), headValue(LOW_PRI+1u), headValue(HIGH_PRI), tailIndex());
  printf("\nactive.low: %u,\tactive.mid: %u,\tactive.high: %u, \tempty: %u",
         (activeStatus(LOW_PRI) == ACTIVE),
         (activeStatus(LOW_PRI+1) == ACTIVE),
         (activeStatus(HIGH_PRI) == ACTIVE),
         PBUF_empty());
  printf("\n");
  printf("  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .\n");
}

//#endif /** DEBUG */
