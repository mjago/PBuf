/** @file */
#ifndef DEFS_H
#define DEFS_H

#if BUFFER_SIZE < 3 || BUFFER_SIZE > 256

# error ERROR: BUFFER_SIZE should be a value from 3 to 256

#endif  /* BUFFER_SIZE */

/**
   The highest priority in the system. */
#define HIGH_PRI PRIORITY_SIZE - 1u

/**
   The lowest priority in the system. */
#define LOW_PRI 0u

/**
   The index_t type holds an index value.
*/

typedef uint8_t index_t;

/**
   The check_t type holds the result of a check.
*/

typedef uint8_t check_t;

/**
   The activity_t type holds 8 bits of data known as activity flags - one bit per priority, to a maximum of
   8 levels of priority. If the buffer currently holds data of a given priority, the appropriate bit is set to
   ACTIVE. Once all elements of a particular quality of data are retrieved from the buffer, the relevant flag
   is set to INACTIVE. The respective head only holds relevant data when the flag is ACTIVE.
*/

typedef uint8_t activity_t;
/**
   The cell_t structure is the buffers composite element. It holds an element and a next variable per slot in
   the buffer. This linkage around the circular buffer enables the buffer to be re-routed or remapped easily,
   allowing for prioritised data to be organised in order of preference. In other words, it can be quickly
   re-arranged to allow higher orders of priority to be retrieved from the buffer quicker than lower orders
   of priority.
*/

typedef struct CELL_T
{

  /**
     data holds the data of the element. */
  element_t data;

  /**
     next is a link pointing to the next element in the buffer. */
  index_t next;
} cell_t;

/**
   The ptr_t structure holds the tail and array of heads pointers' which point into the buffer.
   these are used to ensure buffer access is very fast. The head array size is
   configured by the library user.
*/

typedef struct PTR_T
{
  /**
     tail is a pointer to the buffer element to be read next.
     This is the element having the highest priority in the buffer. */
  index_t tail;

  /**
     head is an array of pointers to the respective heads of the priorities on the buffer.
     These are required to know where to insert future prioritised data. */
  index_t head[PRIORITY_SIZE];
} ptr_t;

/**
   The pbuf_t structure holds the relevant data required for operating a single buffer.
   Its size is determined at compile time and depends upon the configuration applied.
   There is a single 8-bit tail, an 8-bit head for each priority, and an 8-bit activity
   byte for storing an activity flag per priority. In addition there is the buffer itself,
   each cell containing an element of data storage and an 8-bit pointer to the following
   cell.
*/

typedef struct PBUF_T {

  /**
     Tail and Head pointers */
  ptr_t ptr;

  /**
     Array of buffer composite elements */
  cell_t element[BUFFER_SIZE];

  /**
     Storage for activity statuses of priorities in use.
     Activity status may be ACTIVE or INACTIVE. */
  activity_t activity;

} pbuf_t;

enum {
  INVALID_INDEX,
  VALID_INDEX,
  INVALID_ELEMENT,
  VALID_ELEMENT,
  INVALID_PRIORITY,
  VALID_PRIORITY,
  INVALID_WRITE,
  VALID_WRITE,
  INVALID_ACTIVE,
  VALID_ACTIVE,
  INVALID_HEAD,
  VALID_HEAD,
  INVALID_INSERT,
  VALID_INSERT,
  INACTIVE,
  ACTIVE,
  INSERT_FAIL,
  INSERT_SUCCESS,
  BUFFER_NOT_EMPTY,
  BUFFER_EMPTY,
  REROUTE_FAIL,
  REROUTE_SUCCESS,
  BUFFER_NOT_FULL,
  BUFFER_FULL,
  INVALID_REMAP,
  VALID_REMAP,
  INVALID_RETRIEVE,
  VALID_RETRIEVE,
  INVALID_DATA,
  VALID_DATA,
};

#endif  /* DEFS_H */
