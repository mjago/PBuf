#ifndef CLI_H

/* States */

typedef enum {
  STATE_INIT,
  STATE_ENTER_ACTION,
  STATE_ENTER_PRIORITY,
  STATE_ENTER_VALUE,
  STATE_RETRIEVE_VALUE,
  STATE_RESET,
  STATE_SIZE,
  STATE_EXIT
} state_t;

/* State Machine */

extern void PBUF_reset(void);

typedef struct {
  state_t state;
  void (*func)(void);
} stateMachine_t;

enum priority_t {
  HIGH_PRIORITY = 1,
  MID_PRIORITY,
  LOW_PRIORITY,
} priority_t;

enum other_t {
  VALID_NUMBER = 0,
  INVALID_NUMBER,
  VALID_RETRIEVE,
  INVALID_RETRIEVE,
  MENU_SIZE = 7
} other_t;

#define CLI_H
#endif /* CLI_H */
