/** @file */
#ifndef DEFS_H
#define DEFS_H

#if BUFFER_SIZE < 3 || BUFFER_SIZE > 256

# error ERROR: BUFFER_SIZE should be a value from 3 to 256

#endif  /* BUFFER_SIZE */

#define HIGH_PRI PRIORITY_SIZE - 1
#define LOW_PRI 0

/**
   The element_t type holds a buffer element;
*/

typedef uint8_t element_t;

/**
   The priority_t type holds an index value;
*/

typedef uint8_t index_t;

/**
   The priority_t type holds a priority value;
*/

typedef uint8_t priority_t;

/**
   The check_t type holds the result of a check;
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
  element_t data;
  index_t next;
} cell_t;

/**
   The ptr_t structure holds the tail and array of heads pointers' which point into the buffer.
   these are used to ensure buffer access is very fast. The head array size is
   configured by the library user.
*/

typedef struct PTR_T
{
  index_t tail;
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
  ptr_t ptr;
  cell_t element[BUFFER_SIZE];
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
