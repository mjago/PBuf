#ifndef PRIORITY_BUFFER_H
#define PRIORITY_BUFFER_H

#include <inttypes.h>

/**
   Set Buffer Size Here - Size may be anything between 3 and 256 buffer elements */

#ifndef BUFFER_SIZE

#  define BUFFER_SIZE 4

#endif  /* !BUFFER_SIZE */

/**
   Set Number of priority levels Here (8 max) */

#ifndef PRIORITY_SIZE

#  define PRIORITY_SIZE 3

#endif  /* !PRIORITY_SIZE */

#ifndef PRIORITY_SIZE

#  define PRIORITY_SIZE 3

#endif  /* !PRIORITY_SIZE */

/**
   Set the element size here (8, 16, 32, 0r 64) */

#define ELEMENT_SIZE 8

/**
   The element_t type holds a buffer element.
*/

#if ELEMENT_SIZE == 8

typedef uint8_t element_t;

#elif ELEMENT_SIZE == 16

typedef uint16_t element_t;

#elif ELEMENT_SIZE == 32

typedef uint32_t element_t;

#elif ELEMENT_SIZE == 64

typedef uint64_t element_t;

#else

# error ERROR! ELEMENT_SIZE must be one of 8, 16, 32 or 64

#endif

/**
   define EXTERNAL_DATA_BUFFER if data buffer is to be maintained externally */

  //#define EXTERNAL_DATA_BUFFER

/**
   The priority_t type holds a priority value.
*/

typedef uint8_t priority_t;

int PBUF_reset(void);
int PBUF_empty(void);
int PBUF_full(void);
int PBUF_bufferSize(void);
int PBUF_ElementSize(void);
int PBUF_insert(element_t element, priority_t priority);
int PBUF_retrieve(element_t * element);
int PBUF_insertIndex(int * index, priority_t priority);
int PBUF_retrieveIndex(int * index);

#ifdef UNIT_TESTS

# include "test.h"

# define STATIC

#else

# define STATIC static

#endif  /* UNIT_TESTS */

#ifdef DEBUG

void PBUF_print(void);

#endif /* DEBUG */

#endif /* ! PRIORITY_BUFFER_H */
