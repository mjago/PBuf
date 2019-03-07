#ifndef TEST_H
#define TEST_H

#include <inttypes.h>
#include "unity.h"
#include "defs.h"

#ifndef UNITY
#  error ERROR: test.h exposed in non-unittest mode!
#endif

//////////////////////////////// index ////////////////////////////////

check_t checkIndex(index_t index);
check_t nextIndex(index_t * nextIdx, index_t currentIdx);
check_t writeNextIndex(index_t currentIdx, index_t nextIdx);
check_t firstFreeElementIndex(index_t * index);
index_t headIndex(priority_t priority);
index_t nextHeadIndex(priority_t priority);
check_t writeHead(index_t index, priority_t priority);
index_t tailIndex(void);
check_t nextTailIndex(index_t * index);
check_t incTail(void);
index_t writeTail(index_t index);
index_t lowestPriorityTail(void);
check_t remap(index_t a1, index_t a2, index_t b);
check_t remapNotFull(index_t newIndex, priority_t priority);
index_t insertPointFull(priority_t priority);
index_t insertPointNotFull(priority_t priority);
index_t bridgePointFull(void);
index_t bridgePointNotFull(void);
check_t writeElementIndex(index_t * index, priority_t priority);
check_t overwriteElementIndex(index_t * index, priority_t priority);
check_t insertIndex(index_t * index, priority_t priority);
check_t insertEmptyIndex(index_t * index, priority_t priority);
check_t insertNotFullIndex(index_t * index, priority_t priority);
check_t insertFullIndex(index_t * index, priority_t priority);

//////////////////////////////// priority ////////////////////////////////

check_t validatePriority(priority_t priority);
check_t lowestPriority(priority_t * priority);
check_t highestPriority(priority_t * priority);
check_t nextHighestPriority(priority_t * nextPriority, priority_t priority);
check_t activeStatus(priority_t priority);
check_t setActive(priority_t priority);
check_t setInactive(priority_t priority);
check_t adjustPriority(void);
uint8_t activePriorityCount(void);

//////////////////////////////// element ////////////////////////////////

check_t readData(element_t * element, index_t index);
check_t writeData(element_t element, index_t index);
check_t insert(element_t element, priority_t priority);

check_t resetBufferPointers(void);
check_t resetBuffer(void);
check_t bufferFull(void);
check_t bufferEmpty(void);

#endif /* TEST_H */
