#include <inttypes.h>
#include "priority_buffer.h"
#include "defs.h"

//////////////////////////////// index ////////////////////////////////

STATIC check_t checkIndex(index_t index);
STATIC check_t nextIndex(index_t * nextIdx, index_t currentIdx);
STATIC check_t writeNextIndex(index_t currentIdx, index_t nextIdx);
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
STATIC check_t writeElementIndex(index_t * index, priority_t priority);
STATIC check_t overwriteElementIndex(index_t * index, priority_t priority);
STATIC check_t insertIndex(index_t * index, priority_t priority);
STATIC check_t insertEmptyIndex(index_t * index, priority_t priority);
STATIC check_t insertNotFullIndex(index_t * index, priority_t priority);
STATIC check_t insertFullIndex(index_t * index, priority_t priority);

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

STATIC check_t readData(element_t * element, index_t index);
STATIC check_t writeData(element_t element, index_t index);
STATIC check_t insert(element_t element, priority_t priority);

STATIC check_t resetBufferPointers(void);
STATIC check_t resetBuffer(void);
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

  if((nextTailIndex(&index) == VALID_INDEX) &&
     (writeTail(index) == VALID_INDEX))
    {
      returnVal = VALID_INDEX;
    }

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
  check_t returnVal = INVALID_INDEX;

  if(checkIndex(index) == VALID_INDEX)
    {
      bf.ptr.tail = index;
      returnVal = VALID_INDEX;
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
  check_t returnVal = INVALID_INDEX;

    {
      *index = bf.element[tailIndex()].next;
      if(checkIndex(*index) == VALID_INDEX)
        {
          returnVal = VALID_INDEX;
        }
    }

    return returnVal;
}

/**
   Update the passed in index with the next index linked
   from the current index passed in. Returns index validity.
   \return VALID_INDEX or INVALID_INDEX */

STATIC check_t nextIndex(index_t * nextIdx, index_t currentIdx)
{
  check_t returnVal = INVALID_INDEX;

  if(checkIndex(currentIdx) == VALID_INDEX)
    {
      *nextIdx = bf.element[currentIdx].next;
      returnVal = VALID_INDEX;
    }

  return returnVal;
}

/**
   Writes the index passed in to the element referenced by
   the current index passed in.
   \return VALID_INDEX or INVALID_INDEX */

STATIC check_t writeNextIndex(index_t currentIdx, index_t nextIdx)
{
  check_t returnVal = INVALID_INDEX;

  if((checkIndex(currentIdx) == VALID_INDEX) &&
     (checkIndex(nextIdx) == VALID_INDEX))
    {
      bf.element[currentIdx].next = nextIdx;
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
   \return VALID_WRITE or INVALID_WRITE */

STATIC check_t writeHead(index_t index, priority_t priority)
{
  check_t returnVal = INVALID_WRITE;

  if((validatePriority(priority) == VALID_PRIORITY) &&
     (checkIndex(index) == VALID_INDEX))
    {
      bf.ptr.head[priority] = index;
      returnVal = VALID_WRITE;
    }

  return returnVal;
}

/**
   Determine the lowest priority in the buffer.
   \return VALID_PRIORITY or INVALID_PRIORITY */

STATIC check_t lowestPriority(priority_t * priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t priCount = 0;
  uint8_t mask = 0x01;

  if(bufferEmpty() == BUFFER_NOT_EMPTY)
    {
      for(priCount = LOW_PRI ;priCount < PRIORITY_SIZE ;priCount++)
        {
          if(bf.activity & mask)
            {
              *priority = priCount;
              returnVal = VALID_PRIORITY;
              break;
            }
          mask *= 2u;
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
   \return VALID_PRIORITY or INVALID_PRIORITY */

STATIC check_t highestPriority(priority_t * priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t priCount;
  uint8_t mask = 1 << (PRIORITY_SIZE - 1);

  for(priCount = PRIORITY_SIZE ; priCount > 0; priCount--)
    {
      if(bf.activity & mask)
        {
          *priority = priCount - 1;
          returnVal = VALID_PRIORITY;
          break;
        }

      mask /= 2u;
    }

  return returnVal;
}

/**
   Reset Heads and Tail
   \return VALID_RESET or INVALID_RESET */

STATIC check_t resetBufferPointers(void)
{
  check_t returnVal = VALID_RESET;
  priority_t count;

  bf.activity = 0u;
  if(writeTail(BUFFER_SIZE - 1) == VALID_INDEX)
    {
      for(count = LOW_PRI; count < PRIORITY_SIZE; count++)
        {
          if(writeHead(BUFFER_SIZE - 1u, count) != VALID_WRITE)
            {
              returnVal = INVALID_RESET;
              break;
            }
        }
    }

  return returnVal;
}

/**
   Reset the Buffer
   \return VALID_RESET or INVALID_RESET */

STATIC check_t resetBuffer(void)
{
  check_t returnVal = VALID_RESET;
  uint16_t count;

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      if( ! ((writeData(0u, count) == VALID_ELEMENT) &&
             (writeNextIndex(count, (count + 1u) % BUFFER_SIZE) == VALID_INDEX)))
        {
          returnVal = INVALID_RESET;
          break;
        }
    }

  return returnVal;
}

STATIC check_t writeElementIndex(index_t * index, priority_t priority)
{
  check_t returnVal = INVALID_WRITE;
  priority_t lowestPri;

  if(bufferFull() == BUFFER_NOT_FULL)
    {
      if(bufferEmpty() == BUFFER_EMPTY)
        {
          nextTailIndex(index);
        }
      else
        {
          lowestPriority(&lowestPri);
          nextIndex(index, headIndex(lowestPri));
        }

      if((writeHead(*index, priority)) &&
         (setActive(priority) == VALID_ACTIVE))
        {
          returnVal = VALID_WRITE;
        }
    }

  return returnVal;
}

//////////////////////////////// element ////////////////////////////////

/**
   Find the next highest priority to the priority we wish to insert - used to find
   where to store the new priority
   \return VALID_PRIORITY or INVALID_PRIORITY */

check_t nextHighestPriority(priority_t * nextPriority, priority_t priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t priCount;
  priority_t mask = 1u << (priority + 1);

  for(priCount = priority + 1u; priCount < PRIORITY_SIZE; priCount++)
    {
      if(bf.activity & mask)
        {
          *nextPriority = priCount;
          returnVal = VALID_PRIORITY;
          break;
        }
      mask *= 2;
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
  priority_t mask = 1u;

  for(priority = LOW_PRI; priority < PRIORITY_SIZE; priority++)
    {
      if(bf.activity & mask)
        {
          returnVal++;
        }
      mask *= 2;
    }

  return returnVal;
}

/**
   Determines the lowest priority tail index
   \return index of the lowest priority tail */

STATIC index_t lowestPriorityTail(void)
{
  priority_t lowestButOnePri;
  priority_t lowestPri;
  index_t lowestTail;

  lowestPriority(&lowestPri);
  nextHighestPriority(&lowestButOnePri, lowestPri);
  nextIndex(&lowestTail, headIndex(lowestButOnePri));
  return lowestTail;
}

/**
   Determine index of next insert and modify index variable passed in.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t insertIndex(index_t * index, priority_t priority)
{
  check_t returnVal = INVALID_INSERT;
  priority_t lowestPri;

  if(bufferEmpty() == BUFFER_EMPTY)
    {
      if(insertEmptyIndex(index, priority) == VALID_INSERT)
        {
          returnVal = VALID_INSERT;
        }
    }

  else if(bufferFull() == BUFFER_FULL)
    {
      if(insertFullIndex(index, priority) == VALID_INSERT)
        {
          returnVal = VALID_INSERT;
        }
    }

  else
    {
      // buffer neither full nor empty
      if((lowestPriority(&lowestPri) == VALID_PRIORITY) &&
         (priority > lowestPri))
        {
          if(insertNotFullIndex(index, priority) == VALID_INSERT)
            {
              returnVal = VALID_INSERT;
            }
        }
      else
        {
          if(writeElementIndex(index, priority) == VALID_WRITE)
            {
              returnVal = VALID_INSERT;
            }
        }

      returnVal = VALID_INSERT;
    }
  return returnVal;
}

/**
   Insert an element into the buffer of a given priority and
   adjust the buffer to correct the prioritisation if required.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t insert(element_t element, priority_t priority)
{
  check_t returnVal = INVALID_INSERT;
  index_t index;

  if(insertIndex(&index, priority) == VALID_INSERT)
    {
      if(writeData(element, index) == VALID_ELEMENT)
        {
          returnVal = VALID_INSERT;
        }
    }

  return returnVal;
}

#ifndef EXTERNAL_DATA_BUFFER

/**
   Write the element passed in to the index passed in.
   Check the index is within the bounds of the buffer.
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

#endif  /* ! EXTERNAL_DATA_BUFFER */

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

  if( ! bf.activity)
    {
      returnVal = BUFFER_EMPTY;
    }

  return returnVal;
}

STATIC check_t insertEmptyIndex(index_t * index, priority_t priority)
{
  check_t returnVal = INVALID_INSERT;

  if(writeElementIndex(index, priority) == VALID_WRITE)
    {
      returnVal = VALID_INSERT;
    }

  return returnVal;
}

/**
   Insert into a not empty not full buffer.
   Remaps the buffer to correct the prioritisation.
   Modifies the index to reflect the insert point.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t insertNotFullIndex(index_t * index, priority_t priority)
{
  check_t returnVal = INVALID_INSERT;

  if(firstFreeElementIndex(index) == VALID_INDEX)
    {
      if(remapNotFull(*index, priority) == VALID_REMAP)
        {
          returnVal = VALID_INSERT;
        }
    }

  return returnVal;
}

/**
   Insert into a full buffer. Remaps the buffer to correct the
   prioritisation.
   Modifies the index to reflect the insert point.
   \return VALID_INSERT or INVALID_INSERT */

STATIC check_t insertFullIndex(index_t * index, priority_t priority)
{
  check_t returnVal = INVALID_INSERT;
  priority_t lowestPri;

  // if lower or equal priority data in buffer find lowest
  if((lowestPriority(&lowestPri)) == VALID_PRIORITY)
    {
      if(lowestPri <= priority)
        {
          // overwrite oldest element at lowest priority
          if(overwriteElementIndex(index, priority) == VALID_WRITE)
            {
              returnVal = VALID_INSERT;
            }
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

STATIC check_t readElementIndex(index_t * index)
{
  check_t returnVal = INVALID_ELEMENT;

  if(nextTailIndex(index) == VALID_INDEX)
    {
      if((adjustPriority() == VALID_PRIORITY) &&
         (writeTail(*index) == VALID_INDEX))
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
      if((nextIndex(&a1ptr, a1) == VALID_INDEX) &&
         (nextIndex(&bptr, b) == VALID_INDEX) &&
         (nextIndex(&a2ptr, a2) == VALID_INDEX))
        {
          // don't remap if it is not needed
          if(a1ptr != b)
            {
              if((writeNextIndex(a1, b) == VALID_INDEX) &&
                 (nextIndex(&bptr, b) == VALID_INDEX) &&
                 (writeNextIndex(a2, bptr) == VALID_INDEX) &&
                 (writeNextIndex(b, a1ptr) == VALID_INDEX))
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

  if((writeHead(newIndex, priority) == VALID_WRITE) &&
     (setActive(priority) == VALID_ACTIVE))
    {
      if(remap(insertPt, bridgePt, newIndex) == VALID_REMAP)
        {
          if(tailIndex() != newIndex)
            {
              returnVal = VALID_REMAP;
            }
          else
            {
              if(writeTail(bridgePt) == VALID_INDEX)
                {
                  returnVal = VALID_REMAP;
                }
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
  check_t returnVal = INVALID_REMAP;
  index_t insertPt = insertPointFull(priority);
  index_t bridgePt = bridgePointFull();
  index_t lowestPriTailIdx = lowestPriorityTail();
  priority_t lowestPri;

  if((writeHead(index, priority) == VALID_WRITE) &&
     (setActive(priority) == VALID_ACTIVE))
    {
      if(remap(insertPt, bridgePt, index))
        {
          if(index == tailIndex())
            {
              if((lowestPriority(&lowestPri) == VALID_PRIORITY) &&
                 (lowestPri != priority) &&
                 (lowestPriTailIdx  == tailIndex()))
                {
                  setInactive(lowestPri);
                }
            }

          // reset tail to possibly new lowest pri
          if(lowestPriority(&lowestPri) == VALID_PRIORITY)
            {
              if(writeTail(headIndex(lowestPri)) == VALID_INDEX)
                {
                  returnVal = VALID_REMAP;
                }
            }
        }
    }

  return returnVal;
}

/**
   Overwrite index without remapping if buffer is full and only
   a single priority exists on the buffer
   \return VALID_WRITE or INVALID_WRITE */

STATIC check_t overwriteSinglePriorityIndex(index_t * index, priority_t priority)
{
  check_t returnVal = INVALID_WRITE;

  if(nextTailIndex(index) == VALID_INDEX)
    {
      if(writeHead(*index, priority) == VALID_WRITE)
        {
          if(activeStatus(priority) == ACTIVE)
            {
              incTail();
            }
          else
            {
              setActive(priority);
            }

          returnVal = VALID_WRITE;
        }
    }

  return returnVal;
}

/**
   Overwrite index since buffer is full and there are no unused elements.
   \return VALID_WRITE or INVALID_WRITE */

STATIC check_t overwriteElementIndex(index_t * index, priority_t priority)
{
  check_t returnVal = INVALID_WRITE;
  priority_t priorityCount = activePriorityCount();

  if(priorityCount == 1)
    {
      returnVal = overwriteSinglePriorityIndex(index, priority);
    }
  else
    {
      *index = lowestPriorityTail();
      if(remapFull(*index, priority) == VALID_REMAP)
        {
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
   Reset Buffer.
   \return zero on successful reset */

int PBUF_reset(void)
{
  return ! ((resetBufferPointers() == VALID_RESET) &&
            (resetBuffer() == VALID_RESET));
}

/**
   Check if buffer is empty.
   \return non-zero if buffer is empty.
*/

int PBUF_empty(void)
{
  return (bufferEmpty() == BUFFER_EMPTY);
}

/**
   Check if buffer is full.
   \return non-zero if buffer is full.
*/

int PBUF_full(void)
{
  return bufferFull() == BUFFER_FULL;
}

/**
   Return the size of buffer.
   \return size of buffer
*/

int PBUF_bufferSize(void)
{
  return BUFFER_SIZE;
}


#ifndef EXTERNAL_DATA_BUFFER

/**
   Insert data into the buffer of the given priority.
   \return zero for a valid insert.
   \return non-zero for an invalid insert. */

int PBUF_insert(element_t element, priority_t priority)
{
  return ! (insert(element, priority) == VALID_INSERT);
}

/**
   Retrieve an element from the tail of the buffer and assign to
   the element pointer passed in.
   \return zero on successful retrieve.
   \return non-zero on failed retrieve. */

int PBUF_retrieve(element_t * element)
{
  check_t returnVal = INVALID_RETRIEVE;
  index_t index;

  if( ! PBUF_empty())
    {
      if((readElementIndex(&index) == VALID_ELEMENT) &&
         (readData(element, index) == VALID_ELEMENT))
        {
          returnVal = VALID_RETRIEVE;
        }
    }
  return returnVal;
}

#endif  /* ! EXTERNAL_DATA_BUFFER */

/**
   Insert index of the given priority is passed to by
   modifying index. Zero is returned for a successful
   insert.
   \return zero for a valid insert.
   \return non-zero for an invalid insert. */

int PBUF_insertIndex(int * index, priority_t priority)
{
  return ! (insertIndex((index_t *) index, priority) == VALID_INSERT);
}

/**
   Retrieve index is passed to the caller by modifying index.
   Zero returned if a valid retrieve has happened.
   \return zero on successful retrieve.
   \return non-zero on failed retrieve. */

int PBUF_retrieveIndex(int * index)
{
  check_t returnVal = INVALID_RETRIEVE;
  index_t tempIndex;

  if(bufferEmpty() == BUFFER_EMPTY)
    {
      if(readElementIndex(&tempIndex) == VALID_ELEMENT)
        {
          *index = (int) tempIndex;
          returnVal = VALID_RETRIEVE;
        }
    }
  return returnVal;
}

/** @} */
/* end of API group */

//////////////////////////////// Debug ////////////////////////////////

#ifdef DEBUG

#include <stdio.h>

#ifndef EXTERNAL_DATA_BUFFER

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
          printf("%u", bf.element[index].data);
          count++;
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

#endif  /* ! EXTERNAL_DATA_BUFFER */

/** @} */
/* end of Debug group */

#endif /** DEBUG */
