#include "priority_buffer.h"
#include "defs.h"
#include "unity.h"
#include "unity_fixture.h"

#define MID_PRI 1
#define TEST_ASSERT_ZERO TEST_ASSERT_FALSE

TEST_GROUP(pBuf);

TEST_SETUP(pBuf)
{
  PBUF_reset();
}

TEST_TEAR_DOWN(pBuf)
{
}

TEST(pBuf, bufferFull_returns_BUFFER_FULL_when_buffer_full)
{
  uint8_t count;

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      writeElement(123, LOW_PRI);
    }

  TEST_ASSERT_EQUAL(BUFFER_FULL, bufferFull());
}

TEST(pBuf, bufferFull_returns_BUFFER_NOT_FULL_when_buffer_not_full)
{
  TEST_ASSERT_EQUAL(BUFFER_NOT_FULL, bufferFull());
}

TEST(pBuf, bufferEmpty_returns_BUFFER_EMPTY_following_a_reset)
{
  TEST_ASSERT_EQUAL(BUFFER_EMPTY, bufferEmpty());
}

TEST(pBuf, bufferEmpty_returns_BUFFER_NOT_EMPTY_on_activity)
{
  setActive(HIGH_PRI);
  TEST_ASSERT_EQUAL(BUFFER_NOT_EMPTY, bufferEmpty());
}

TEST(pBuf, PBUF_empty_should_return_non_zero_following_a_reset)
{
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, PBUF_empty_should_return_TRUE_following_a_reset)
{
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, writeElement_should_write_the_passed_element_to_the_buffer)
{
  TEST_ASSERT_EQUAL(VALID_WRITE, writeElement(123, LOW_PRI));
}

TEST(pBuf, activeStatus_should_return_INACTIVE_for_all_priorities_after_reset)
{
  uint8_t count;

  for(count = 0; count < PRIORITY_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(INACTIVE, activeStatus(count));
    }
}

TEST(pBuf, activeStatus_should_return_ACTIVE_for_all_priorities_after_they_are_set)
{
  uint8_t count;

  for(count = 0; count < PRIORITY_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_ACTIVE, setActive(count));
      TEST_ASSERT_EQUAL(ACTIVE, activeStatus(count));
    }
}

TEST(pBuf, setAactive_should_return_INVALID_ACTIVE_when_passed_an_invalid_priority)
{
  uint8_t count;

  for(count = 0; count < PRIORITY_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_ACTIVE, setActive(count));
      TEST_ASSERT_EQUAL(ACTIVE, activeStatus(count));
    }
}

TEST(pBuf, validatePriority_should_return_VALID_PRIORITY_when_passed_valid_priority)
{
  uint8_t count;

  for(count = 0; count < PRIORITY_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_PRIORITY, validatePriority(count));
    }
}

TEST(pBuf, validatePriority_should_return_INVALID_PRIORITY_when_passed_an_invalid_priority)
{
  uint8_t result = validatePriority(PRIORITY_SIZE);
  TEST_ASSERT_EQUAL(INVALID_PRIORITY, result);
}

TEST(pBuf, PBUF_bufferSize_should_return_correct_buffer_size)
{
  uint16_t size = PBUF_bufferSize();
  TEST_ASSERT_EQUAL(BUFFER_SIZE, size);
}

TEST(pBuf, setHead_should_set_head)
{
  TEST_ASSERT_EQUAL(INVALID_HEAD, setHead(0, PRIORITY_SIZE));
}

TEST(pBuf, incHead_should_increment_head)
{
  TEST_ASSERT_EQUAL(VALID_HEAD, incHead(0));
  TEST_ASSERT_EQUAL(0, headValue(0));
  TEST_ASSERT_EQUAL(VALID_HEAD, incHead(0));
  TEST_ASSERT_EQUAL(1, headValue(0));
  TEST_ASSERT_EQUAL(VALID_HEAD, incHead(0));
  TEST_ASSERT_EQUAL(2, headValue(0));
  TEST_ASSERT_EQUAL(VALID_HEAD, incHead(1));
  TEST_ASSERT_EQUAL(0, headValue(1));
  TEST_ASSERT_EQUAL(VALID_HEAD, incHead(1));
  TEST_ASSERT_EQUAL(1, headValue(1));
}

TEST(pBuf, firstfreeElementIndex_should_return_next_tail_with_an_empty_buffer)
{
  uint8_t index;

  TEST_ASSERT_EQUAL(VALID_INDEX, firstFreeElementIndex(&index));
  TEST_ASSERT_EQUAL(0, index);
}

TEST(pBuf, firstfreeElementIndex_should_return_the_correct_index_when_elements_in_buffer)
{
  uint8_t index;

  writeElement(42, HIGH_PRI);
  TEST_ASSERT_EQUAL(VALID_INDEX, firstFreeElementIndex(&index));
  TEST_ASSERT_EQUAL(1, index);

  PBUF_reset();

  writeElement(42, 1);
  TEST_ASSERT_EQUAL(VALID_INDEX, firstFreeElementIndex(&index));
  TEST_ASSERT_EQUAL(1, index);

  PBUF_reset();

  writeElement(42, HIGH_PRI);
  TEST_ASSERT_EQUAL(VALID_INDEX, firstFreeElementIndex(&index));
  TEST_ASSERT_EQUAL(1, index);
  writeElement(43, LOW_PRI);
  TEST_ASSERT_EQUAL(VALID_INDEX, firstFreeElementIndex(&index));
  TEST_ASSERT_EQUAL(2, index);
  writeElement(44, HIGH_PRI);
  TEST_ASSERT_EQUAL(VALID_INDEX, firstFreeElementIndex(&index));
  TEST_ASSERT_EQUAL(2, index);
}

TEST(pBuf, writeElement_should_write_element)
{
  TEST_ASSERT_EQUAL(VALID_WRITE, writeElement(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_WRITE, writeElement(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_WRITE, writeElement(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_WRITE, writeElement(45, HIGH_PRI));
  TEST_ASSERT_EQUAL(BUFFER_FULL, bufferFull());
}

TEST(pBuf, writeElement_should_set_priority)
{
  TEST_ASSERT_EQUAL(VALID_WRITE, writeElement(42, LOW_PRI));
}

TEST(pBuf, writeElement_should_set_head)
{
  TEST_ASSERT_EQUAL(VALID_WRITE, writeElement(42, LOW_PRI));
}

TEST(pBuf, insert_priC_should_return_INSERT_SUCCESS)
{
  uint16_t count;
  uint8_t value;

  insert(42, LOW_PRI);
  insert(43, LOW_PRI);
  insert(44, LOW_PRI);
  insert(45, LOW_PRI);
  insert(46, LOW_PRI);
  insert(47, LOW_PRI);
  insert(48, LOW_PRI);

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_INSERT, insert(count % BUFFER_SIZE, LOW_PRI));
    }

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
      TEST_ASSERT_EQUAL(count, value);
    }
}

TEST(pBuf, readElement_should_read_the_next_element)
{
  uint8_t element;
  writeElement(42, LOW_PRI);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&element));
  TEST_ASSERT_EQUAL(42, element);
}

TEST(pBuf, PBUB_retrieve_should_retrieve_the_next_element)
{
  uint8_t element;
  writeElement(42, LOW_PRI);
  TEST_ASSERT_ZERO(PBUF_retrieve(&element));
  TEST_ASSERT_EQUAL(42, element);
}

TEST(pBuf, PBUB_retrieve_should_return_INVALID_ELEMENT_if_the_buffer_is_empty)
{
  uint8_t element;
  TEST_ASSERT_EQUAL(INVALID_RETRIEVE, PBUF_retrieve(&element));
}

TEST(pBuf, insertEmpty_should_insert_to_the_beginning_of_the_empty_buffer)
{
  insertEmpty(42, LOW_PRI);
}

TEST(pBuf, insertNotFull_inserts_regardless_of_priority_to_the_end_of_the_buffer)
{
  writeElement(42, HIGH_PRI);
  writeElement(43, HIGH_PRI);
  insertNotFull(44, LOW_PRI);
}

TEST(pBuf, insertFull_should_insert_over_lower_priority_data)
{
  element_t element;

  writeElement(42, HIGH_PRI);
  writeElement(43, HIGH_PRI);
  writeElement(44, LOW_PRI);
  writeElement(45, LOW_PRI);
  insertFull(69, HIGH_PRI);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readNextElement(&element));
  TEST_ASSERT_EQUAL(69, element);
  PBUF_reset();
}

TEST(pBuf, insert_priB_should_return_VALID_INSERT)
{
  uint16_t count;
  uint8_t value;
  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_INSERT, insert(count % BUFFER_SIZE, MID_PRI));
    }
  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
      TEST_ASSERT_EQUAL(count, value);
    }
}

TEST(pBuf, insert_priA_should_return_VALID_INSERT)
{
  uint16_t count;
  uint8_t value;
  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_INSERT, insert(count % BUFFER_SIZE, HIGH_PRI));
    }

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
      TEST_ASSERT_EQUAL(count, value);
    }
}

TEST(pBuf, insert_priC_should_return_overwrite_on_wraparound_VALID_INSERT)
{
  uint16_t count;
  uint8_t value;

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_INSERT, insert(count % BUFFER_SIZE, LOW_PRI));
    }
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));

  for(count = 0; count < BUFFER_SIZE - 1; count++)
    {
      TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
      TEST_ASSERT_EQUAL((count + 1) % BUFFER_SIZE, value);
    }
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
}

TEST(pBuf, insert_priB_should_return_overwrite_on_wraparound_VALID_INSERT)
{
  uint16_t count;
  uint8_t value;

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_INSERT, insert(count % BUFFER_SIZE, MID_PRI));
    }
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));

  for(count = 0; count < BUFFER_SIZE - 1; count++)
    {
      TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
      TEST_ASSERT_EQUAL((count + 1) % BUFFER_SIZE, value);
    }
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
}

TEST(pBuf, insert_priA_should_return_INSERT_FAIL_on_wraparound_VALID_INSERT)
{
  uint16_t count;
  uint8_t value;

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_INSERT, insert(count % BUFFER_SIZE, HIGH_PRI));
    }

  TEST_ASSERT_EQUAL(INSERT_FAIL, insert(42, HIGH_PRI));

  for(count = 0; count < BUFFER_SIZE; count++)
    {
      TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
      TEST_ASSERT_EQUAL((count) % BUFFER_SIZE, value);
    }
}

TEST(pBuf, insert_priA_PriB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_PriC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priB_sequence_should_be_reprioritised)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priA_sequence_should_be_reprioritised)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priC_priA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priC_priB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priC_priC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priB_PriA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priB_PriC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priA_PriA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priA_PriB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priA_PriC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priA_priB_PriB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priC_priA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priC_priB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priC_priC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priB_priA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priB_priB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priB_priC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priA_priA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priA_priB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priB_priA_priC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priA_priA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priA_priB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priA_priC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priB_priA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priB_priB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priB_priC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priC_priA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priC_priB_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priC_priC_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, insert_priC_priB_priCA_priC_priB_priCA_sequence_should_be_in_order)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(45, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(44, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(43, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(42, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(45, value);
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(46, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(47, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(47, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(46, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, saturation_sequence_low_priority)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(45, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(46, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(47, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(48, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(49, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(50, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(51, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(48, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(49, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(50, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(51, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, saturation_sequence_mid_priority)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(45, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(46, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(47, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(48, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(49, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(50, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(51, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(48, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(49, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(50, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(51, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, saturation_sequence_high_priority)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(42, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(43, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(44, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(45, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(46, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(47, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(48, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(49, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(50, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(51, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(48, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(49, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(50, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(51, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, solePriority_should_return_VALID_PRIORITY_if_single_priority_in_buffer)
{
  TEST_ASSERT_EQUAL(INVALID_PRIORITY, solePriority());
  setActive(HIGH_PRI);
  TEST_ASSERT_EQUAL(VALID_PRIORITY, solePriority());
  setActive(MID_PRI);
  TEST_ASSERT_EQUAL(INVALID_PRIORITY, solePriority());
  setInactive(MID_PRI);
  TEST_ASSERT_EQUAL(VALID_PRIORITY, solePriority());
  setActive(LOW_PRI);
  TEST_ASSERT_EQUAL(INVALID_PRIORITY, solePriority());
  setInactive(HIGH_PRI);
  TEST_ASSERT_EQUAL(VALID_PRIORITY, solePriority());
}

TEST(pBuf, add_high_priority_to_buffer_full_of_low)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(20, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(21, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(22, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(23, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(24, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(25, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(25, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(22, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(23, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(24, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, add_mid_priority_to_buffer_full_of_low)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(20, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(21, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(22, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(23, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(24, LOW_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(25, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(25, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(22, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(23, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(24, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, add_high_priority_to_buffer_full_of_mid)
{
  uint8_t value;

  TEST_ASSERT_EQUAL(VALID_INSERT, insert(20, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(21, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(22, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(23, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(24, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(25, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(25, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(22, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(23, value);
  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
  TEST_ASSERT_EQUAL(24, value);
  TEST_ASSERT_TRUE(PBUF_empty());
}

TEST(pBuf, add_mid_priority_to_buffer_full_of_high_returns_INVALID_INSERT)
{
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(20, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(21, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(22, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(23, HIGH_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(24, HIGH_PRI));
  TEST_ASSERT_EQUAL(INVALID_INSERT, insert(25, MID_PRI));
}

TEST(pBuf, add_low_priority_to_buffer_full_of_mid_returns_INVALID_INSERT)
{
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(20, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(21, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(22, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(23, MID_PRI));
  TEST_ASSERT_EQUAL(VALID_INSERT, insert(24, MID_PRI));
  TEST_ASSERT_EQUAL(INVALID_INSERT, insert(25, LOW_PRI));
}

//TEST(pBuf, pL_pL_pL_pL_pL_pH_pH_should_resequence_correctly)
//{
//  uint8_t value;
//
//  TEST_ASSERT_EQUAL(VALID_INSERT, insert(20, LOW_PRI));
//  TEST_ASSERT_EQUAL(VALID_INSERT, insert(21, LOW_PRI));
//  TEST_ASSERT_EQUAL(VALID_INSERT, insert(22, LOW_PRI));
//  TEST_ASSERT_EQUAL(VALID_INSERT, insert(23, LOW_PRI));
//  TEST_ASSERT_EQUAL(VALID_INSERT, insert(24, LOW_PRI));//oldest
//  TEST_ASSERT_EQUAL(VALID_INSERT, insert(25, LOW_PRI));
//  PBUF_print();
//  TEST_ASSERT_EQUAL(VALID_INSERT, insert(26, HIGH_PRI));
//  PBUF_print();
//  TEST_ASSERT_EQUAL(VALID_INSERT, insert(27, HIGH_PRI));
//  PBUF_print();
//  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
//  TEST_ASSERT_EQUAL(26, value);
//  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
////  TEST_ASSERT_EQUAL(27, value);
////  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
////  TEST_ASSERT_EQUAL(24, value);
////  TEST_ASSERT_EQUAL(VALID_ELEMENT, readElement(&value));
////  TEST_ASSERT_EQUAL(25, value);
////  TEST_ASSERT_TRUE(PBUF_empty());
//}
