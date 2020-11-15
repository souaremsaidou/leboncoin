#include "fizzbuzz.hpp"
#include "gtest/gtest.h"

#include <vector>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(FizzbuzzTest, TestOk) {
  std::vector<std::string> expected = {
      "1",    "2",    "fizz",     "4",    "buzz", "fizz",
      "7",    "8",    "fizz",     "buzz", "11",   "fizz",
      "13",   "14",   "fizzbuzz", "16",   "17",   "fizz",
      "19",   "buzz", "fizz",     "22",   "23",   "fizz",
      "buzz", "26",   "fizz",     "28",   "29",   "fizzbuzz",
      "31",   "32",   "fizz",     "34",   "buzz", "fizz",
      "37",   "38",   "fizz",     "buzz", "41",   "fizz",
      "43",   "44",   "fizzbuzz", "46",   "47",   "fizz",
      "49",   "buzz", "fizz",     "52",   "53",   "fizz",
      "buzz", "56",   "fizz",     "58",   "59",   "fizzbuzz",
      "61",   "62",   "fizz",     "64",   "buzz", "fizz",
      "67",   "68",   "fizz",     "buzz", "71",   "fizz",
      "73",   "74",   "fizzbuzz", "76",   "77",   "fizz",
      "79",   "buzz", "fizz",     "82",   "83",   "fizz",
      "buzz", "86",   "fizz",     "88",   "89",   "fizzbuzz",
      "91",   "92",   "fizz",     "94",   "buzz", "fizz",
      "97",   "98",   "fizz",     "buzz"};
  ASSERT_EQ(expected, fizzbuzz(3, 5, 100, "fizz", "buzz"));
}

TEST(FizzbuzzTest, TestkO) {
  std::vector<std::string> expected = {
      "1",    "2",    "fizz", "4",    "buzz", "fizz", "7",        "8",
      "fizz", "buzz", "11",   "fizz", "13",   "14",   "fizzbuzz", "16"};
  ASSERT_NE(expected, fizzbuzz(3, 5, 16, "", ""));
}