#ifndef CLI_H

/* States */

typedef enum {
  STATE_INIT,
  STATE_ENTER_ACTION,
  STATE_ENTER_PRIORITY,
  STATE_RETRIEVE_VALUE,
  STATE_RESET,
  STATE_EXIT
} state_t;

/* State Machine */

typedef struct {
  state_t state;
  void (*func)(void);
} stateMachine_t;

enum {
  LOW_PRIORITY = 0,
  MID_PRIORITY,
  HIGH_PRIORITY,
};

enum {
  VALID_NUMBER = 0,
  INVALID_NUMBER,
  VALID_RETRIEVE,
  INVALID_RETRIEVE,
  MENU_SIZE = 6
};

#define CLI_H
#endif /* CLI_H */
