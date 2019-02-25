#ifndef PRIORITY_BUFFER_H
#define PRIORITY_BUFFER_H

/* Set Buffer Size Here - Size may be anything between 3 and 256 buffer elements */
#define BUFFER_SIZE 4

/* Set Buffer Count Here (256 max) */
#define BUFFER_COUNT 1

/* Set Number of priority levels Here (8 max) */
#define PRIORITY_SIZE 3

#ifdef UNIT_TESTS

# include "test.h"

# define STATIC

#else

# define STATIC static

#endif  /* UNIT_TESTS */

void PBUF_reset(void);
uint8_t PBUF_empty(void);
uint8_t PBUF_full(void);
uint16_t PBUF_bufferSize(void);
uint8_t PBUF_ElementSize(void);
uint8_t PBUF_insert(uint8_t element, uint8_t priority);
uint8_t PBUF_retrieve(uint8_t * element);

//#ifdef DEBUG
void PBUF_print(void);
//#endif /* DEBUG */

#endif /* ! PRIORITY_BUFFER_H */
