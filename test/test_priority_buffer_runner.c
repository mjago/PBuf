#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(pBuf)
{
  RUN_TEST_CASE(pBuf, bufferFull_returns_BUFFER_FULL_when_buffer_full);
  RUN_TEST_CASE(pBuf, bufferFull_returns_BUFFER_NOT_FULL_when_buffer_not_full);
  RUN_TEST_CASE(pBuf, bufferEmpty_returns_BUFFER_EMPTY_following_a_reset);
  RUN_TEST_CASE(pBuf, bufferEmpty_returns_BUFFER_NOT_EMPTY_on_activity);
  RUN_TEST_CASE(pBuf, PBUF_empty_should_return_non_zero_following_a_reset);
  RUN_TEST_CASE(pBuf, writeElement_should_write_the_passed_element_to_the_buffer);
  RUN_TEST_CASE(pBuf, activeStatus_should_return_INACTIVE_for_all_priorities_after_reset);
  RUN_TEST_CASE(pBuf, activeStatus_should_return_ACTIVE_for_all_priorities_after_they_are_set);
  RUN_TEST_CASE(pBuf, setAactive_should_return_INVALID_ACTIVE_when_passed_an_invalid_priority);
  RUN_TEST_CASE(pBuf, validatePriority_should_return_VALID_PRIORITY_when_passed_valid_priority);
  RUN_TEST_CASE(pBuf, validatePriority_should_return_INVALID_PRIORITY_when_passed_an_invalid_priority);
  RUN_TEST_CASE(pBuf, PBUF_bufferSize_should_return_correct_buffer_size);
  RUN_TEST_CASE(pBuf, setHead_should_set_head);
  RUN_TEST_CASE(pBuf, incHead_should_increment_head);
  RUN_TEST_CASE(pBuf, firstfreeElementIndex_should_return_next_tail_with_an_empty_buffer);
  RUN_TEST_CASE(pBuf, firstfreeElementIndex_should_return_the_correct_index_when_elements_in_buffer);
  RUN_TEST_CASE(pBuf, readElement_should_read_the_next_element);
  RUN_TEST_CASE(pBuf, PBUB_retrieve_should_retrieve_the_next_element);
  RUN_TEST_CASE(pBuf, PBUB_retrieve_should_return_INVALID_ELEMENT_if_the_buffer_is_empty);
  RUN_TEST_CASE(pBuf, insertEmpty_should_insert_to_the_beginning_of_the_empty_buffer);
  RUN_TEST_CASE(pBuf, insertNotFull_inserts_regardless_of_priority_to_the_end_of_the_buffer);
  RUN_TEST_CASE(pBuf, writeElement_should_write_element);
  RUN_TEST_CASE(pBuf, writeElement_should_set_priority);
  RUN_TEST_CASE(pBuf, insert_priC_should_return_INSERT_SUCCESS);
  RUN_TEST_CASE(pBuf, insert_priA_PriB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_PriC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priB_sequence_should_be_reprioritised);
  RUN_TEST_CASE(pBuf, insert_priB_priA_sequence_should_be_reprioritised);
  RUN_TEST_CASE(pBuf, insert_priA_priA_PriA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priA_priA_PriB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priA_priA_PriC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priA_priB_PriA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priA_priB_PriB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priA_priB_PriC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priA_priC_priA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priA_priC_priB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priA_priC_priC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priA_priA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priA_priB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priA_priC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priB_priA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priB_priB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priB_priC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priC_priA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priC_priB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priB_priC_priC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priA_priA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priA_priB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priA_priC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priB_priA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priB_priB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priB_priC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priC_priA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priC_priB_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priC_priC_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, insert_priC_priB_priCA_priC_priB_priCA_sequence_should_be_in_order);
  RUN_TEST_CASE(pBuf, saturation_sequence_low_priority);
  RUN_TEST_CASE(pBuf, saturation_sequence_mid_priority);
  RUN_TEST_CASE(pBuf, saturation_sequence_high_priority);
  RUN_TEST_CASE(pBuf, insertFull_should_insert_over_lower_priority_data);
  RUN_TEST_CASE(pBuf, solePriority_should_return_VALID_PRIORITY_if_single_priority_in_buffer);
  RUN_TEST_CASE(pBuf, add_high_priority_to_buffer_full_of_low);
  RUN_TEST_CASE(pBuf, add_mid_priority_to_buffer_full_of_low);
  RUN_TEST_CASE(pBuf, add_high_priority_to_buffer_full_of_mid);
  RUN_TEST_CASE(pBuf, add_mid_priority_to_buffer_full_of_high_returns_INVALID_INSERT);
  RUN_TEST_CASE(pBuf, add_low_priority_to_buffer_full_of_mid_returns_INVALID_INSERT);
  // RUN_TEST_CASE(pBuf, pL_pL_pL_pL_pL_pH_pH_should_resequence_correctly);
}



