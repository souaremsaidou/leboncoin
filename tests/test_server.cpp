#include "server.hpp"
#include "gtest/gtest.h"

#include <vector>

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

const std::string host("127.0.0.1");
const int port = 1984;

TEST(ServerTest, TestOk) {
  Server server(host, port);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  server.stop();
  ASSERT_EQ(0, 0);
}