#ifdef ARDUINO
#include <Arduino.h>
#endif
#include <unity.h>
#include "logic_core.h"

void test_percent_basic() {
  TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, lc_percentElapsed(1500,1500));
  TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.5f, lc_percentElapsed(750,1500));
  TEST_ASSERT_FLOAT_WITHIN(0.0001f, 1.0f, lc_percentElapsed(0,1500));
}

void test_percent_bounds() {
  TEST_ASSERT_EQUAL_FLOAT(1.0f, lc_percentElapsed(-10, 100));
  TEST_ASSERT_EQUAL_FLOAT(1.0f, lc_percentElapsed(0, 0));
}

void test_segment_index() {
  // total=1000, segments=25 => segLength=40
  TEST_ASSERT_EQUAL(0, lc_segmentIndex(1000,1000,25)); // start
  TEST_ASSERT_EQUAL(0, lc_segmentIndex(999,1000,25));
  TEST_ASSERT_EQUAL(1, lc_segmentIndex(960,1000,25));
  TEST_ASSERT_EQUAL(24, lc_segmentIndex(5,1000,25));
}

void test_format_time() {
  char buf[6];
  lc_formatTime(0, buf); TEST_ASSERT_EQUAL_STRING("00:00", buf);
  lc_formatTime(59, buf); TEST_ASSERT_EQUAL_STRING("00:59", buf);
  lc_formatTime(60, buf); TEST_ASSERT_EQUAL_STRING("01:00", buf);
  lc_formatTime(5999, buf); // 99:59 max (clamped)
  TEST_ASSERT_EQUAL_STRING("99:59", buf);
}

void test_minutes_seconds() {
  TEST_ASSERT_EQUAL(25, lc_minutes(1500));
  TEST_ASSERT_EQUAL(0, lc_seconds(1500));
  TEST_ASSERT_EQUAL(24, lc_minutes(1499));
  TEST_ASSERT_EQUAL(59, lc_seconds(1499));
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_percent_basic);
  RUN_TEST(test_percent_bounds);
  RUN_TEST(test_segment_index);
  RUN_TEST(test_format_time);
  RUN_TEST(test_minutes_seconds);
  UNITY_END();
}

void loop() {}
