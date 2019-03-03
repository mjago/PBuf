#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "cli.h"

#include "../src/priority_buffer.h"

#define PRI_HIGH (PRIORITY_SIZE - 1)

/* States */

static void stateInit(void);
static void stateEnterAction(void);
static void stateEnterPriority(void);
static void stateRetrieveValue(void);
static void stateReset(void);
static void stateExit(void);
static uint8_t readNumber(uint8_t *element);
static void printPriority(void);
static void printBuffer(void);
static void printMenu(void);
static void clearTerminal(void);
static void insertValue(int value);

/* State Machine */

static stateMachine_t stateMachine[] =
  {
    {STATE_INIT, stateInit},
    {STATE_ENTER_ACTION, stateEnterAction},
    {STATE_ENTER_PRIORITY, stateEnterPriority},
    {STATE_RETRIEVE_VALUE, stateRetrieveValue},
    {STATE_RESET, stateReset},
    {STATE_EXIT, stateExit},
  };

/* static local state */

static state_t state;
static uint8_t priority;
static int inputValue;

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
  priority = LOW_PRIORITY;
  state = STATE_ENTER_ACTION;
  PBUF_reset();
  clearTerminal();
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
      clearTerminal();
      printf("Enter a Priority between 0 and %u \n (0 is lowest, %u is highest)...\n\n",PRI_HIGH, PRI_HIGH);
      if(readNumber(&value) == VALID_NUMBER)
        {
          if(value < PRIORITY_SIZE)
            {
              priority = (uint8_t) value;
              state = STATE_ENTER_ACTION;
              break;
            }
        }
    }

  clearTerminal();
  printBuffer();
}

static void stateEnterAction(void)
{
  int value;
  char tmp[256]={0x0};
  char str[16], *endp;

  while(fgets(tmp, sizeof(tmp), stdin)!=NULL)
    {
      if((tmp[0] == 'p') || (tmp[0] == 'P'))
        {
          state = STATE_ENTER_PRIORITY;
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
      else if((tmp[0] == 'q') || (tmp[0] == 'Q'))
        {
          state = STATE_EXIT;
          break;
        }
      else if((tmp[0] >= '0') && (tmp[0] <= '9'))
        {
          inputValue = strtoumax(tmp, &endp, 0);
          insertValue(inputValue);
          break;
        }
      else
        {
          clearTerminal();
          printBuffer();
          state = STATE_ENTER_ACTION;
          break;
        }
    }
}

static void stateRetrieveValue(void)
{
  uint8_t value;

  clearTerminal();
  for(;;)
    {
      if(PBUF_retrieve(&value) == 0u)
        {
          printf("Value %d retrieved from buffer\n...\n", value);
        }
      else
        {
          printf("Oops! Unable to retrieve a value!\n...\n");
        }
      printBuffer();
      state = STATE_ENTER_ACTION;
      break;
    }
}

static void stateReset(void)
{
  PBUF_reset();
  clearTerminal();
  printf("Buffer Reset!\n...\n");
  printBuffer();
  state = STATE_ENTER_ACTION;
}

static void stateExit(void)
{
  printf("Quitting!\n");
}

static void printPriority(void)
{
  printf("priority %u ", priority);
  switch(priority)
    {
    case (PRIORITY_SIZE - 1):
      printf("(%s)\n", "highest"); break;
    case 0:
      printf("(%s)\n", "lowest"); break;
    default:
      printf("\n"); break;
    }
}

static void printBuffer(void)
{
  printPriority();
  PBUF_print();
  printMenu();
}

static void printMenu(void)
{
  uint8_t count;
  static const char * space = "  \t";
  static const char * strMenu[MENU_SIZE] =
    {"\n\nEnter either: \t1. data (a number less than 256)",
     "2. one of the following Commands:\n",
     "P : Priority Setting",
     "G : Get Value",
     "R : Reset Buffer",
     "Q : Quit\n"};

  for(count = 0; count < MENU_SIZE; count++)
    {
      printf("%s", space);
      if(count > 0) printf("%s", space);
      printf("%s\n", strMenu[count]);
    }
}

static void clearTerminal(void)
{
  printf("\e[1;1H\e[2J\tPrioritised Buffer Exerciser\n\n\n");
}

static void insertValue(int value)
{
  if(inputValue < 256)
    {
      state = STATE_ENTER_ACTION;
      clearTerminal();
      if(!PBUF_insert((uint8_t) inputValue, priority))
        {
          printf("Inserted %u, ", inputValue);
        }
      else
        {
          printf("Failed to insert %u, ", inputValue);
        }
          printPriority();
      printf("...\n");
      printBuffer();
    }
  else
    {
      clearTerminal();
      printf("Enter a value less than 256!\n...\n");
      printBuffer();
    }
}
