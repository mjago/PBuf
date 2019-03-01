#ifndef PRIORITY_BUFFER_H
#define PRIORITY_BUFFER_H
#include <inttypes.h>

/**
   Set Buffer Size Here - Size may be anything between 3 and 256 buffer elements */

#define BUFFER_SIZE 4

/**
   Set Buffer Count Here (256 max) */

#define BUFFER_COUNT 1

/**
   Set Number of priority levels Here (8 max) */

#define PRIORITY_SIZE 3

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

#elif ELEMENT_SIZE == 32

typedef uint64_t element_t;

#else

# error ERROR! ELEMENT_SIZE must be one of 8, 16, 32 or 64

#endif

/**
   The priority_t type holds a priority value.
*/

typedef uint8_t priority_t;

void PBUF_reset(void);
int PBUF_empty(void);
int PBUF_full(void);
int PBUF_bufferSize(void);
int PBUF_ElementSize(void);
int PBUF_insert(element_t element, priority_t priority);
int PBUF_retrieve(element_t * element);

#ifdef UNIT_TESTS

# include "test.h"

# define STATIC

#else

# define STATIC static

#endif  /* UNIT_TESTS */

//#ifdef DEBUG
void PBUF_print(void);
//#endif /* DEBUG */

#endif /* ! PRIORITY_BUFFER_H */
