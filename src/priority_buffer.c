/** @file */
#include <stdio.h>
#include <inttypes.h>
#include "priority_buffer.h"
#include "defs.h"

/* index */

STATIC check_t checkIndex(index_t index);
STATIC check_t nextIndex(index_t * index, priority_t priority);
STATIC check_t writeNextIndex(index_t current, index_t next);
STATIC check_t incTail(void);
STATIC index_t tailIndex(void);
STATIC index_t writeTail(index_t index);
STATIC check_t nextTailIndex(index_t * index);
STATIC check_t firstFreeElementIndex(index_t * index);
STATIC check_t remapNotFull(index_t newIndex, priority_t priority);
STATIC index_t headValue(priority_t priority);
STATIC check_t remap(index_t a1, index_t a2, index_t b);

/* priority */

STATIC void resetBufferPointers(void);
STATIC void resetBuffer(void);
STATIC index_t nextHeadValue(priority_t priority);
STATIC check_t incHead(priority_t priority);
STATIC check_t setHead(index_t index, priority_t priority);
STATIC check_t lowestPriority(priority_t * priority);
STATIC check_t validatePriority(priority_t priority);
STATIC check_t equalOrHigherPriority(priority_t * equalOrHigherPri, check_t priority);
STATIC check_t activeStatus(priority_t priority);
STATIC priority_t virtualMasterHead(priority_t * priority);
STATIC check_t highestPriority(priority_t * priority);
STATIC check_t setActive(priority_t priority);
STATIC check_t setInactive(priority_t priority);
STATIC check_t nextHighestPriority(priority_t * nextPriority, priority_t priority);
STATIC check_t solePriority(priority_t priority);

/* element */

STATIC check_t writeElement(element_t element, priority_t priority);
STATIC check_t overwriteElement(element_t element, priority_t priority, priority_t newPriority);
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
   The pbuf_t structure holds the relevant data required for operating a single buffer.
   Its size is determined at compile time and depends upon the configuration applied.
   There is a single 8-bit tail, an 8-bit head for each priority, and an 8-bit activity
   byte for storing an activity flag per priority. In addition there is the buffer itself,
   each cell containing an element of data storage and an 8-bit pointer to the following
   cell.
 */

typedef struct PBUF_T {

  struct PTR
  {
    index_t tail;
    index_t head[PRIORITY_SIZE];
  }
  /**
     The ptr structure holds the tail and array of heads pointers' which point into the buffer.
     these are used to ensure buffer access is very fast. The head array size is
     configured by the library user.
  */
    ptr;

  struct cell_t
  {
    element_t data;
    index_t next;
  }
  /**
     The element structure is the buffer itself. It holds an element and a next variable per slot in the buffer.
     This linkage around the circular buffer enables the buffer to be re-routed or remapped easily, allowing for
     prioritised data to be organised in order of preference. In other words, it can be quickly re-arranged
     to allow higher orders of priority to be retrieved from the buffer quicker than lower orders of priority.
  */
    element[BUFFER_SIZE];

  /**
     The activity storage holds 8 bits of data known as activity flags - one bit per priority, to a maximum of
     8 levels of priority. If the buffer currently holds data of a given priority, the appropriate bit is set to
     ACTIVE. Once all elements of a particular quality of data are retrieved from the buffer, the relevant flag
     is set to INACTIVE. The respective head only holds relevant data when the flag is ACTIVE.
  */
  activity_t activity;

} pbuf_t;

STATIC pbuf_t bf;

/**
  Check the index is a valid Index
 */

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
*/

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
  Return the index pointed to by the tail pointer
 */

STATIC index_t tailIndex(void)
{
  return bf.ptr.tail;
}

/**
  Write a new index to the tail pointer
 */

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
 */

STATIC check_t nextTailIndex(index_t * index)
{
  *index = bf.element[tailIndex()].next;
  return VALID_INDEX;
}

/**
  Update the passed in index with the next index linked
  from the current index passed in. Returns index validity.
 */

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
  the current index passed in. Returns index validity.
 */

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
  Returns INVALID_INDEX if we cannot allocate an index (buffer full for instance).
*/

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
      if(virtualMasterHead(&priority) == VALID_PRIORITY)
        {
          *index = nextHeadValue(priority);
          returnVal = VALID_INDEX;
        }
    }

  return returnVal;
}

 /////////////////////////// priority ///////////////////////////

/**
  Return the index pointed to by the head related with the priority passed in.
 */

STATIC index_t headValue(priority_t priority)
{
  return bf.ptr.head[priority];
}

/**
  Return the index pointed to by the head related to the priority passed in
  after following the head link.
 */

STATIC index_t nextHeadValue(priority_t priority)
{
  return (bf.element[headValue(priority)].next);
}

/**
  Advances the head to the next head value by following the buffer pointers
 */

STATIC check_t incHead(priority_t priority)
{
  check_t returnVal = INVALID_HEAD;

  if(setHead(nextHeadValue(priority), priority) == VALID_HEAD)
    {
      returnVal = VALID_HEAD;
    }

return returnVal;
}

/**
  Set the value of the head pointer associated with the priority passed
  in with the index passed in.
 */

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
  Return the head of the lowest active priority. Beyond this
  point there is no valid data unless the buffer is full.
 */

STATIC priority_t virtualMasterHead(priority_t * priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t count;

  if(bufferEmpty() == BUFFER_NOT_EMPTY)
    {
      for(count = LOW_PRI ; ; count--)
        {
          if(activeStatus(count) == ACTIVE)
            {
              *priority = count;
              returnVal = VALID_PRIORITY;
              break;
            }
          else if(count == HIGH_PRI)
            {
              break;
            }
        }
    }

  return returnVal;
}

/**
  Determine the lowest priority in the buffer.
*/

STATIC check_t lowestPriority(priority_t * priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t count;

  for(count = LOW_PRI ; ; count--)
    {
      if(activeStatus(count) == ACTIVE)
        {
          *priority = count;
          returnVal = VALID_PRIORITY;
          break;
        }
      if(count == HIGH_PRI)
        break;
    }

  return returnVal;
}

/**
  Check the priority is a valid one.
 */

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
  Determine the priority that is equal to or higher than the priority passed in,
  and assign it to the priority storage passed in.
  If this priority cannot be found return INVALID_PRIORITY.
 */

STATIC check_t equalOrHigherPriority(priority_t * equalOrHigherPri, check_t priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t count;

  for(count = priority; ; count--)
    {
      if(activeStatus(count) == ACTIVE)
        {
          *equalOrHigherPri = count;
          returnVal = VALID_PRIORITY;
          break;
        }
      if(count == HIGH_PRI)
        {
          break;
        }
    }
  return returnVal;
}

/**
  Return the active status of the priority passed in.
  If the priority is active ACTIVE is returned.
*/

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
  Return INVALID_ACTIVE if the priority is invalid.
 */

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
  Return INVALID_ACTIVE if the priority is invalid.
*/

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
  Returns VALID_PRIORITY on finding a relevant priority.
 */

STATIC check_t highestPriority(priority_t * priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t count;

  for(count = HIGH_PRI; count < PRIORITY_SIZE; count++)
    {
      if(activeStatus(count) == ACTIVE)
        {
          *priority = count;
          returnVal = VALID_PRIORITY;
          break;
        }
    }

  return returnVal;
}

/**
  Reset head and tail pointers.
 */

STATIC void resetBufferPointers(void)
{
  check_t count;
  bf.ptr.tail = BUFFER_SIZE - 1;

  for(count = 0; count < PRIORITY_SIZE; count++)
    {
      bf.ptr.head[count] = BUFFER_SIZE - 1u;
    }
}

/**
  Reset the Buffer.
 */

STATIC void resetBuffer(void)
{
  uint16_t count;

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      bf.element[count].data = 0u;
      bf.element[count].next = (count + 1) % BUFFER_SIZE;
    }

  for(count = 0; count < PRIORITY_SIZE; count++)
    {
      setInactive(count);
    }
}

/** element */

/**
  Write an element to the buffer at the next empty position in the
  linked buffer and adjust the relevant priority head. Returns VALID_WRITE
  on success;
*/

STATIC check_t writeElement(element_t element, priority_t priority)
{
  check_t returnVal = INVALID_WRITE;
  index_t index;
  priority_t virtualHead;

  if((validatePriority(priority) == VALID_PRIORITY) &&
     (bufferFull() == BUFFER_NOT_FULL))
    {
      if(virtualMasterHead(&virtualHead) != VALID_PRIORITY)
        {
          nextTailIndex(&index);
        }
      else
        {
          nextIndex(&index, headValue(virtualHead));
          //          index = tailIndex();
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
  Find the next highest priority to the priority we wish to insert - used to find
   where to store the new priority
*/

check_t nextHighestPriority(priority_t * nextPriority, priority_t priority)
{
  check_t returnVal = INVALID_PRIORITY;
  priority_t count;

  for(count = priority - 1 ; ; count--)
    {
      if(activeStatus(count) == ACTIVE)
        {
          *nextPriority = count;
          returnVal = VALID_PRIORITY;
          break;
        }
      if(count == HIGH_PRI)
        {
          break;
        }
    }

  return returnVal;
}

/** overwrite oldest element at given priority */

STATIC check_t solePriority(priority_t priority)
{
  check_t returnVal = INVALID_PRIORITY;

  if(validatePriority(priority) == VALID_PRIORITY)
    {
      if((bf.activity) == (1 << priority))
        {
          returnVal = VALID_PRIORITY;
        }
    }

  return returnVal;
}

/** Returns the index of the lowest priority tail */

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
  Used when the buffer is full.
  Overwrite the oldest lowest priority element with the element passed in.
  The lowest priority is passed in since it is to hand, and the new priority
  is passed in and used to determine wheter we need to remap the buffer to
  re-prioritise the data. Returns VALID_WRITE on a successful write.
 */

STATIC check_t overwriteElement(element_t element, priority_t lowestPri, priority_t newPriority)
{
  check_t returnVal = INVALID_WRITE;
  index_t tailIdx;
  index_t lowestPriTailIdx = lowestPriorityTail();
  index_t nextHighestHeadIdx;
  index_t equalOrHigherPriHead;
  priority_t nextHighestPri;
  priority_t equalOrHigherPri;

  nextTailIndex(&tailIdx);

  // is buffer full of single priority?
  if(solePriority(newPriority) == VALID_PRIORITY)
    {
      // buffer full of same priority
      writeData(element, tailIdx);
      setHead(tailIdx, newPriority);
      incTail();
      returnVal = VALID_WRITE;
    }
  else
    {
      // buffer full of multiple priorities
      nextHighestPriority(&nextHighestPri, lowestPri);
      nextHighestHeadIdx = headValue(nextHighestPri);
      equalOrHigherPriority(&equalOrHigherPri, newPriority);
      equalOrHigherPriHead = headValue(equalOrHigherPri);
      writeData(element, lowestPriTailIdx);

      // is there a single element of the lowest priority and we are higher?
      if((headValue(lowestPri) == lowestPriTailIdx) &&
         (lowestPri != newPriority))
        {
          // overwrite single element and set inactive
          setInactive(lowestPri);
          setHead(lowestPriTailIdx, newPriority);
          setActive(newPriority);
          remap(equalOrHigherPriHead,nextHighestHeadIdx,headValue(newPriority));
          writeTail(nextHighestHeadIdx);
        }
      else if((activeStatus(newPriority) == ACTIVE) &&
              (lowestPri == newPriority))
        {
          // are there already entries of the same priority as the new entry
          //  where we are the lowest priority?
          remap(nextHighestHeadIdx, lowestPriTailIdx, headValue(newPriority));
          setHead(lowestPriTailIdx, newPriority);
          writeTail(lowestPriTailIdx);
        }
      else
        {
          // we are adding the highest priority so insert after tail
          nextHighestPriority(&nextHighestPri, lowestPri);
          nextTailIndex(&tailIdx);
          setHead(lowestPriTailIdx, newPriority);
          remap(tailIndex(), nextHighestHeadIdx, lowestPriTailIdx);
        }

      returnVal = VALID_WRITE;
    }

  return returnVal;
}

/**
  Read the next element and assign to the element pointer passed in.
  Checks that there is data to read. Returns VALID_ELEMENT on success.
 */
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
  Returns VALID_INSERT if the insert is valid.
 */

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
      if((virtualMasterHead(&virtualHead) == VALID_PRIORITY) &&
         (newPriority < virtualHead))
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
  Write the element passed in to the index passed in. Checks
  the index is within the bounds of the buffer. Returns VALID_INDEX
  for a valid write.
 */

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
  parameter: none
  returns: BUFFER_FULL if the buffer is full
  returns: BUFFER_NOT_FULL if the buffer is not full
*/

STATIC check_t bufferFull(void)
{
  check_t returnVal = BUFFER_NOT_FULL;
  priority_t priority;

  for(priority = HIGH_PRI ; priority <= LOW_PRI; priority++)
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
  Check whether the buffer is empty. Returns either BUFFER_EMPTY or
   BUFFER_NOT_EMPTY.
*/

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
  buffer. Returns VALID_INSERT if successful.
 */

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
  prioritisation. Returns VALID_INSERT if successful.
 */

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
  prioritisation. Returns VALID_INSERT if successful.
 */

STATIC check_t insertFull(element_t element, priority_t priority)
{
  check_t returnVal = INVALID_INSERT;
  priority_t lowPri;

  // if lower or equal priority data in buffer find lowest
  if((lowestPriority(&lowPri)) == VALID_PRIORITY)
    {
      if(lowPri >= priority)
        {
          // overwrite oldest element at lowest priority
          overwriteElement(element, lowPri, priority);

          returnVal = VALID_INSERT;
        }
    }

  return returnVal;
}

/**
  Adjust the tail, and make any depleted priorities inactive.
  Returns VALID_HEAD with success.
 */

STATIC check_t adjustHeads(void)
{
  check_t returnVal = VALID_HEAD;
  priority_t priority;
  index_t index;
  index_t tailIdx;

  nextTailIndex(&tailIdx);
  for(priority = HIGH_PRI ; priority < PRIORITY_SIZE; priority++)
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
    }

  return returnVal;
}

/**
  Read element from the buffer and assign to the
  element pointer passed in. Returns VALID_ELEMENT on
  successful read.
 */

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
  save a1*
  a1* = b
  a2* = b*
  b* = saved a1*

  Returns VALID_REMAP on success.
*/

STATIC check_t remap(index_t a1, index_t a2, index_t b)
{
  check_t returnVal = INVALID_REMAP;
  index_t a1ptr;
  index_t bptr;

  if((checkIndex(a1) == VALID_INDEX) &&
     (checkIndex(a2) == VALID_INDEX) &&
     (checkIndex(b) == VALID_INDEX))
    {
      nextIndex(&a1ptr, a1);
      writeNextIndex(a1, b);
      nextIndex(&bptr, b);
      writeNextIndex(a2, bptr);
      writeNextIndex(b, a1ptr);
      returnVal = VALID_REMAP;
    }

  return returnVal;
}

/**
  Remap buffer for a not full buffer for the index of priority
  passed in. Return VALID_PRIORITY on success.
 */

STATIC check_t remapNotFull(index_t newIndex, priority_t priority)
{
  check_t returnVal = INVALID_REMAP;
  index_t tailIdx;
  index_t a1;
  index_t a1ptr;
  index_t a2;
  priority_t virtualPriority;
  priority_t highestPri;

  if(highestPriority(&highestPri) == VALID_PRIORITY)
    {
      if(highestPri <= priority)
        {
          nextIndex(&a1ptr, headValue(highestPri));
          a1 = headValue(highestPri);
        }
      else
        {
          nextIndex(&tailIdx, tailIndex());
          a1ptr = tailIdx;
          a1 = tailIndex();
        }

      if(virtualMasterHead(&virtualPriority) == VALID_PRIORITY)
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

          remap(a1, a2, newIndex);
          returnVal = VALID_REMAP;
        }
    }

  return returnVal;
}

/// Exposed API /

/**
  Reset Buffer
 */

void PBUF_reset(void)
{
  resetBufferPointers();
  resetBuffer();
}

/**
  Return non-zero if buffer is empty.
 */

check_t PBUF_empty(void)
{
  return (bufferEmpty() == BUFFER_EMPTY);
}

/**
  Return non-zero if the buffer is full.
 */

check_t PBUF_full(void)
{
  return bufferFull() == BUFFER_FULL;
}

/**
  Return the size of the buffer.
 */

uint16_t PBUF_bufferSize(void)
{
  return BUFFER_SIZE;
}

/**
  Insert data into the buffer of the given priority.
  Return zero for a valid insert.
 */
check_t PBUF_insert(element_t element, priority_t priority)
{
  check_t returnVal = INVALID_INSERT;

  if(validatePriority(priority) == VALID_PRIORITY)
    {
      returnVal = !(insert(element, priority) == VALID_INSERT);
    }

  return returnVal;
}

/**
  Retrieve an element from the tail of the buffer and assign to
  the element pointer passed in.
  Returns zero on success.
 */
check_t PBUF_retrieve(element_t * element)
{
  check_t returnVal = INVALID_RETRIEVE;

  if( ! PBUF_empty())
    {
      returnVal = !(readNextElement(element) == VALID_ELEMENT);
    }
  return returnVal;
}

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
      virtualMasterHead(&vmh);
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
          //  } while(count < 4);
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
  printf("\nhead[high]: %u,\thead[mid]: %u,\thead[low]: %u,\ttail: %u,",
         headValue(0), headValue(1), headValue(2), tailIndex());
  printf("\nactive.high: %u,\tactive.mid: %u,\tactive.low: %u, \tempty: %u",
         activeStatus(0), activeStatus(1), activeStatus(2), PBUF_empty());
  printf("\n");
  printf("  .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .\n");
}

//#endif /** DEBUG */
