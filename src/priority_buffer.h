#ifndef PRIORITY_BUFFER_H
#define PRIORITY_BUFFER_H
#include <inttypes.h>

/* Set Buffer Size Here - Size may be anything between 3 and 256 buffer elements */

#define BUFFER_SIZE 4

/* Set Buffer Count Here (256 max) */

#define BUFFER_COUNT 1

/* Set Number of priority levels Here (8 max) */

#define PRIORITY_SIZE 3

/**
   The element_t type holds a buffer element.
*/

typedef uint8_t element_t;

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
