#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "cli.h"
#include "../src/priority_buffer.h"

/* States */

static void stateInit(void);
static void stateEnterAction(void);
static void stateEnterPriority(void);
static void stateEnterValue(void);
static void stateRetrieveValue(void);
static void stateReset(void);
static void stateSize(void);
static void stateExit(void);
static uint8_t readNumber(uint8_t *element);
static void printPriority(void);
static void printBuffer(void);
static void printMenu(void);

/* State Machine */

static stateMachine_t stateMachine[] =
  {
    {STATE_INIT, stateInit},
    {STATE_ENTER_ACTION, stateEnterAction},
    {STATE_ENTER_PRIORITY, stateEnterPriority},
    {STATE_ENTER_VALUE, stateEnterValue},
    {STATE_RETRIEVE_VALUE, stateRetrieveValue},
    {STATE_RESET, stateReset},
    {STATE_SIZE, stateSize},
    {STATE_EXIT, stateExit},
  };

/* static local */
static state_t state;
static uint8_t priority = LOW_PRIORITY;
/* main loop */
int main()
{
  int returnVal = 1;

  state = STATE_INIT;
  for(;;)
    {
      (*stateMachine[state].func)();
      if(state == STATE_EXIT)
        {
          return 0;
        }
    }
  return returnVal;
}


/* State Functions */

static void stateInit(void)
{
  printf("\nTended Prioritised Buffer Evaluation Application\n");
  priority = LOW_PRIORITY;
  state = STATE_ENTER_ACTION;
  PBUF_reset();
  printBuffer();
}

static uint8_t readNumber(uint8_t *element)
{
  uint8_t returnVal = INVALID_NUMBER;
  char str[16], *endp;

  fgets(str, sizeof(str), stdin);
  *element = strtoumax(str, &endp, 0);
  if(*endp == '\n' || *endp == '\0')
    {
      returnVal = VALID_NUMBER;
    }
  return returnVal;
}

static void stateEnterPriority(void)
{
  uint8_t value;
  for(;;)
    {
      printf("\nEnter a Priority between 1 and 3 (1 is highest)...\n\n");
      if(readNumber(&value) == VALID_NUMBER)
        {
          if((value >= 1) && (value <= 3))
            {
              priority = (uint8_t) value;
              printPriority();
              state = STATE_ENTER_ACTION;
              break;
            }
        }
    }
}

static void stateEnterValue(void)
{
  uint8_t value;

  for(;;)
    {
      printBuffer();
      printf("\nEnter a number between 0 and 255\n");
      printf("\n Or M for Menu...\n");
      value = 255;
      if(readNumber(&value) == VALID_NUMBER)
        {
          if((value >= 0) && (value <= 255))
            {
              printf("\ninserting %u (priority %u)\n", value, priority);
              PBUF_insert((uint8_t) value, priority - 1);
              printBuffer();
            }
        }
      else
        {
          state = STATE_ENTER_ACTION;
          break;
        }
    }
}

static void stateEnterAction(void)
{
  int value;
  char tmp[256]={0x0};

  printMenu();

  while(fgets(tmp, sizeof(tmp), stdin)!=NULL)
    {
      if((tmp[0] == 'p') || (tmp[0] == 'P'))
        {
          state = STATE_ENTER_PRIORITY;
          break;
        }
      else if((tmp[0] == 'e') || (tmp[0] == 'E'))
        {
          state = STATE_ENTER_VALUE;
          break;
        }
      else if((tmp[0] == 'g') || (tmp[0] == 'G'))
        {
          state = STATE_RETRIEVE_VALUE;
          break;
        }
      else if((tmp[0] == 'r') || (tmp[0] == 'R'))
        {
          state = STATE_RESET;
          break;
        }
      else if((tmp[0] == 's') || (tmp[0] == 'S'))
        {
          state = STATE_SIZE;
          break;
        }
      else if((tmp[0] == 'q') || (tmp[0] == 'Q'))
        {
          state = STATE_EXIT;
          break;
        }
      else
        {
          state = STATE_ENTER_ACTION;
          break;
        }
    }
}

static void stateRetrieveValue(void)
{
  uint8_t value;
  for(;;)
    {
      if(PBUF_retrieve(&value) == VALID_RETRIEVE)
        {
          printf("\nValue %d removed from buffer\n", value);
        }
      else
        {
          printf("\nOops! Unable to remove a value!\n");
        }
      printBuffer();
      state = STATE_ENTER_ACTION;
      break;
    }
}

static void stateReset(void)
{
  printf("Resetting Buffer!\n");
  PBUF_reset();
  printBuffer();
  state = STATE_ENTER_ACTION;
}

static void stateExit(void)
{
  printf("Quitting!\n");
}

static void stateSize(void)
{
  printf("Size!\n");
}

static void printPriority(void)
{
  printf("\nPriority %u ", priority);
  switch(priority)
    {
    case 1:
      printf("(%s).\n", "high"); break;
    case 2:
      printf("(%s).\n", "medium"); break;
    case 3:
      printf("(%s).\n", "low"); break;
    default:
      priority = 3;
      break;
    }
}

static void printBuffer(void)
{
  printPriority();
  PBUF_print();
}

static void printMenu(void)
{
  uint8_t count;
  static const char * space = "  ";
  static const char * strMenu[7] =
    {"\nEnter one of the following:",
     "E : Enter Values",
     "P : Priority",
     "G : Get Value",
     "R : Reset Buffer",
     "S : Show Size",
     "Q : Quit\n"};

  for(count = 0; count < MENU_SIZE; count++)
    {
      printf("%s", space);
      if(count > 0) printf("%s", space);
      printf("%s\n", strMenu[count]);
    }
}
