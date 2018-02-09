#include "./lib/unity/unity.h"

void example_test()
{
  int a = 1;
  TEST_ASSERT( a == 1 ); //this one will pass
}

void example_test2()
{
  int a = 1;
  TEST_ASSERT( a == 2 ); //this one will pass
}

int main(int argc, char** argv) {
  printf("%s\n", argv[1]);
  UNITY_BEGIN();
    RUN_TEST(example_test);
    RUN_TEST(example_test2);
  return UNITY_END();
}
