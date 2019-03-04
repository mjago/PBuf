#include <stdio.h>
#include <inttypes.h>
#include "priority_buffer.h"
#include "defs.h"

//////////////////////////////// index ////////////////////////////////

STATIC check_t checkIndex(index_t index);
STATIC check_t nextIndex(index_t * index, priority_t priority);
STATIC check_t writeNextIndex(index_t current, index_t next);
STATIC check_t firstFreeElementIndex(index_t * index);
STATIC index_t headIndex(priority_t priority);
STATIC index_t nextHeadIndex(priority_t priority);
STATIC check_t writeHead(index_t index, priority_t priority);
STATIC index_t tailIndex(void);
STATIC check_t nextTailIndex(index_t * index);
STATIC check_t incTail(void);
STATIC index_t writeTail(index_t index);
STATIC index_t lowestPriorityTail(void);
STATIC check_t remap(index_t a1, index_t a2, index_t b);
STATIC check_t remapNotFull(index_t newIndex, priority_t priority);
STATIC index_t insertPointFull(priority_t priority);
STATIC index_t insertPointNotFull(priority_t priority);
STATIC index_t bridgePointFull(void);
STATIC index_t bridgePointNotFull(void);

//////////////////////////////// priority ////////////////////////////////

STATIC check_t validatePriority(priority_t priority);
STATIC check_t lowestPriority(priority_t * priority);
STATIC check_t highestPriority(priority_t * priority);
STATIC check_t nextHighestPriority(priority_t * nextPriority, priority_t priority);
STATIC check_t activeStatus(priority_t priority);
STATIC check_t setActive(priority_t priority);
STATIC check_t setInactive(priority_t priority);
STATIC check_t adjustPriority(void);
STATIC uint8_t activePriorityCount(void);

//////////////////////////////// element ////////////////////////////////

STATIC check_t writeElement(element_t element, priority_t priority);
STATIC check_t overwriteElement(element_t element, priority_t newPriority);
STATIC check_t readElement(element_t * element);
STATIC check_t readData(element_t * element, index_t index);
STATIC check_t writeData(element_t element, index_t index);
STATIC check_t insert(element_t element, priority_t priority);
STATIC check_t insertEmpty(element_t element, priority_t newPriority);
STATIC check_t insertNotFull(element_t element, priority_t newPriority);
STATIC check_t insertFull(element_t element, priority_t newPriority);

STATIC void resetBufferPointers(void);
STATIC void resetBuffer(void);
STATIC check_t bufferFull(void);
STATIC check_t bufferEmpty(void);

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
      if(writeTail(index) == VALID_INDEX)
        {
          returnVal = VALID_INDEX;
        }
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
          *index = nextHeadIndex(priority);
          returnVal = VALID_INDEX;
        }
    }

  return returnVal;
}

//////////////////////////////// priority ////////////////////////////////

/**
   Return the index pointed to by the head related with the priority passed in.
   \return the index referenced by the relevant priority head */

STATIC index_t headIndex(priority_t priority)
{
  return bf.ptr.head[priority];
}

/**
   Return the index pointed to by the head related to the priority passed in
   after following the head link.
   \return the index linked to by the relevant priority head */

STATIC index_t nextHeadIndex(priority_t priority)
{
  return (bf.element[headIndex(priority)].next);
}

/**
   Set the value of the head pointer associated with the priority passed
   in with the index passed in.
   \return VALID_HEAD or INVALID_HEAD */

STATIC check_t writeHead(index_t index, priority_t priority)
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

  writeTail(BUFFER_SIZE - 1);

  for(count = LOW_PRI; count < PRIORITY_SIZE; count++)
    {
      writeHead(BUFFER_SIZE - 1u, count);
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
      writeData(0u, count);
      writeNextIndex(count, (count + 1u) % BUFFER_SIZE);
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

  if(bufferFull() == BUFFER_NOT_FULL)
    {
      if(lowestPriority(&virtualHead) != VALID_PRIORITY)
        {
          nextTailIndex(&index);
        }
      else
        {
          nextIndex(&index, headIndex(virtualHead));
        }

      if(writeData(element, index) == VALID_ELEMENT)
        {
          if((writeHead(index, priority)) &&
             (setActive(priority) == VALID_ACTIVE))
            {
              returnVal = VALID_WRITE;
            }
        }
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
  nextIndex(&lowestTail, headIndex(lowestButOnePri));
  return lowestTail;
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
      // buffer neither full nor empty
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
  check_t returnVal = INVALID_ELEMENT;
  if(index < BUFFER_SIZE)
    {
      bf.element[index].data = element;
      returnVal = VALID_ELEMENT;
    }

  return returnVal;
}

/**
   Read the next available data element.
   Check the index is within the bounds of the buffer.
   \return VALID_ELEMENT or INVALID_ELEMENT */

STATIC check_t readData(element_t * element, index_t index)
{
  check_t returnVal = INVALID_ELEMENT;
  if(index < BUFFER_SIZE)
    {
      *element = bf.element[index].data;
      returnVal = VALID_ELEMENT;
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
         (tailIndex() == headIndex(priority)))
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

  if((firstFreeElementIndex(&freeIndex) == VALID_INDEX) &&
     (writeData(element, freeIndex) == VALID_ELEMENT))
    {
      if(remapNotFull(freeIndex, newPriority) == VALID_REMAP)
        {
          returnVal = VALID_INSERT;
        }
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
   Mark the highest priority inactive if necessary.
   \return VALID_PRIORITY or INVALID_PRIORITY */

STATIC check_t adjustPriority(void)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t priority;
  index_t index;

  if((highestPriority(&priority) == VALID_PRIORITY) &&
     (activeStatus(priority) == ACTIVE) &&
     (nextTailIndex(&index) == VALID_INDEX))
    {
      if(headIndex(priority) == index)
        {
          setInactive(priority);
        }

      returnVal = VALID_PRIORITY;
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
      if((readData(element, tailIdx) == VALID_ELEMENT) &&
         (adjustPriority() == VALID_PRIORITY) &&
         (writeTail(tailIdx) == VALID_INDEX))
        {
          returnVal = VALID_ELEMENT;
        }
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
      // don't remap if it is not needed
      if(a1ptr != b)
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
   insertPointNotFull() calculates the index of the valid insert point to be used when remapping the buffer.
   This routine is particularly used when the buffer is not full and an overwrite hasn't taken place.
   The priority passed in is the priority of the newly added element. See the 'Adding data to the Buffer' document
   for more information.
   \return VALID_PRIORITY or INVALID_PRIORITY */

STATIC index_t insertPointNotFull(priority_t priority)
{
  index_t returnVal = 255;
  priority_t highestPri;

  if(highestPriority(&highestPri) == VALID_PRIORITY)
    {
      // priority higher to highest pri in use?
      if(priority > highestPri)
        {
          returnVal = tailIndex();
        }
      else
        {
          returnVal = headIndex(highestPri);
        }
    }

  return returnVal;
}

/**
   Calculate the index of the valid insert point to be used when remapping the buffer.
   This routine is particularly used when an overwrite has taken place due to a full buffer.
   The priority passed in is the priority of the newly added element.
   See 'adding_data_to_the_Buffer' for more information.
   \return insert point index */

index_t insertPointFull(priority_t priority)
{
  index_t returnVal = tailIndex();
  priority_t count;

  for(count = priority; count < PRIORITY_SIZE; count++)
    {
      if(activeStatus(count) == ACTIVE)
        {
          returnVal = headIndex(priority);
          break;
        }
    }
  return returnVal;
}

/**
   Calculate the bridgePoint when the buffer is full. The bridgePoint is passed to the
   remap routine to notify the bridgePoint (see 'adding_data_to_the_Buffer' for more information).
   \return bridge point index */

index_t bridgePointFull(void)
{
  index_t returnVal = tailIndex();
  priority_t count;
  priority_t lowestPri;

  lowestPriority(&lowestPri);

  for(count = lowestPri + 1; count < PRIORITY_SIZE; count++)
    {
      if(activeStatus(count) == ACTIVE)
        {
          returnVal = headIndex(count);
          break;
        }
    }
  return returnVal;
}


/**
   Calculate the bridge point when the buffer is not full. The bridge point is passed to the
   remap routine to indicate the bridge point (see 'adding_data_to_the_Buffer' for more information).
   \return bridge point index */

STATIC index_t bridgePointNotFull(void)
{
  priority_t lowestPri;

  lowestPriority(&lowestPri);
  return headIndex(lowestPri);
}


/**
   Remap buffer for a not full buffer for the index of priority
   passed in.
   \return VALID_PRIORITY or INVALID_PRIORITY */

STATIC check_t remapNotFull(index_t newIndex, priority_t priority)
{
  check_t returnVal = INVALID_REMAP;
  index_t insertPt = insertPointNotFull(priority);
  index_t bridgePt = bridgePointNotFull();

  if((writeHead(newIndex, priority) == VALID_HEAD) &&
     (setActive(priority) == VALID_ACTIVE))
    {
      if(remap(insertPt, bridgePt, newIndex) == VALID_REMAP)
        {
          if(tailIndex() == newIndex)
            {
              if(writeTail(bridgePt) == VALID_INDEX)
                {
                  returnVal = VALID_REMAP;
                }
            }
          else
            {
              returnVal = VALID_REMAP;
            }
        }
    }

  return returnVal;
}

/**
   Remap buffer where the buffer is full relevant to the priority
   passed in.
   \return VALID_PRIORITY or INVALID_PRIORITY */


STATIC check_t remapFull(index_t index, priority_t priority)
{
  check_t returnVal = INVALID_WRITE;
  index_t insertPt = insertPointFull(priority);
  index_t bridgePt = bridgePointFull();
  index_t lowestPriTailIdx = lowestPriorityTail();
  priority_t lowPri;

  if((writeHead(index, priority) == VALID_HEAD) &&
     (setActive(priority) == VALID_ACTIVE))
    {
      if(remap(insertPt, bridgePt, index))
        {
          if(index == tailIndex())
            {
              if((lowestPriority(&lowPri) == VALID_PRIORITY) &&
                 (lowPri != priority) &&
                 (lowestPriTailIdx  == tailIndex()))
                {
                  setInactive(lowPri);
                }
            }

          // reset tail
          if(lowestPriority(&lowPri) == VALID_PRIORITY)
            {
              if(writeTail(headIndex(lowPri)) == VALID_INDEX)
                {
                  returnVal = VALID_WRITE;
                }
            }
        }
    }

  return returnVal;
}

/**
   Overwrite without remapping if buffer is full and only
   a single priority exists on the buffer
   \return VALID_WRITE or INVALID_WRITE */

STATIC check_t overwriteSinglePriority(element_t element, priority_t newPriority)
{
  check_t returnVal = INVALID_WRITE;
  index_t nextTailIdx;

  if(nextTailIndex(&nextTailIdx) == VALID_INDEX)
    {
      if((writeData(element, nextTailIdx) == VALID_ELEMENT) &&
         (writeHead(nextTailIdx, newPriority) == VALID_HEAD))
        {
          if(activeStatus(newPriority) == ACTIVE)
            {
              incTail();
            }
          else
            {
              setActive(newPriority);
            }

          returnVal = VALID_WRITE;
        }
    }

  return returnVal;
}

/**
   Overwrite element since buffer is full and there are no unused elements.
   \return VALID_WRITE or INVALID_WRITE */

STATIC check_t overwriteElement(element_t element, priority_t priority)
{
  check_t returnVal = INVALID_WRITE;
  index_t lowestPriTailIdx = lowestPriorityTail();
  priority_t priorityCount = activePriorityCount();

  if(priorityCount == 1)
    {
      returnVal = overwriteSinglePriority(element, priority);
    }
  else
    {
      if(writeData(element, lowestPriTailIdx) == VALID_ELEMENT)
        {
          if(remapFull(lowestPriTailIdx, priority) == VALID_REMAP)
            returnVal = VALID_WRITE;
        }
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
      returnVal = !(readElement(element) == VALID_ELEMENT);
    }
  return returnVal;
}

/** @} */
/* end of API group */

//////////////////////////////// Debug ////////////////////////////////

#ifdef DEBUG

/**
   @defgroup Debug Debug
   Debug functionality.
   @{

   Print debug output of the buffer and pointer contents.
   Requires DEBUG to be defined at compile time.
*/

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
      lastIndex = headIndex(vmh);
      count = 0;
      do
        {
          if(count > 0)
            {
              printf(" -> ");
            }
          nextIndex(&index, index);
          printf("%u", bf.element[count].data);
          count++;
          //} while(count < 4);
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

  printf("\n");
  for(count = LOW_PRI; count < PRIORITY_SIZE; count++)
    {
      printf("head(%u):   %u, ", count, headIndex(count));
    }
  printf("tail:  %u", tailIndex());

  printf("\n");
  for(count = LOW_PRI; count < PRIORITY_SIZE; count++)
    {
      printf("active(%u): %u, ", count, (activeStatus(count) == ACTIVE));
    }
  printf("empty: %u", PBUF_empty());
  printf("\n");
  printf("  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .\n");
}

/** @} */
/* end of Debug group */

#endif /** DEBUG */
