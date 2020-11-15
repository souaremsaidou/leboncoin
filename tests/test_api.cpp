// System Includes
#include <vector>

// Project Includes
#include "api.hpp"
#include "gtest/gtest.h"

// External Includes

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(ApiTest, TestOk) {
  EXPECT_NO_THROW(StatisticsResource());
  EXPECT_NO_THROW(FizzbuzzResource());
  ASSERT_EQ(0, 0);
}