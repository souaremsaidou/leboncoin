#include "fizzbuzz.hpp"
#include "gtest/gtest.h"

#include <vector>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(FizzbuzzTest, TestOk) {
  std::vector<std::string> expected = {
      "1",    "2",    "fizz", "4",    "buzz", "fizz", "7",        "8",
      "fizz", "buzz", "11",   "fizz", "13",   "14",   "fizzbuzz", "16"};
  ASSERT_EQ(expected, fizzbuzz(3, 5, 16, "fizz", "buzz"));
}

TEST(FizzbuzzTest, TestkO) {
  std::vector<std::string> expected = {
      "1",    "2",    "fizz", "4",    "buzz", "fizz", "7",        "8",
      "fizz", "buzz", "11",   "fizz", "13",   "14",   "fizzbuzz", "16"};
  ASSERT_NE(expected, fizzbuzz(3, 5, 16, "", ""));
}