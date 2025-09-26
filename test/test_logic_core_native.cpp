#include <unity.h>
#include "logic_core.h"

void setUp() {}
void tearDown() {}

void test_percent_native() {
  // remaining=75 of total=300 => elapsed=225 => 0.75
  TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.75f, lc_percentElapsed(75,300));
  // remaining greater than total => elapsed negative clamp => 0
  TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, lc_percentElapsed(400,300));
  // negative remaining => treat as 0 remaining => full elapsed => 1.0
  TEST_ASSERT_FLOAT_WITHIN(0.0001f, 1.0f, lc_percentElapsed(-5,300));
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_percent_native);
  return UNITY_END();
}
