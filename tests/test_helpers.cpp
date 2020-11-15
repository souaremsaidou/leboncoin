// System Includes
#include <unordered_map>
#include <vector>

// Project Includes
#include "fizzbuzz.hpp"
#include "helpers.hpp"
#include "gtest/gtest.h"

// External Includes
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

// External Namespaces
using namespace rapidjson;

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(HelpersTest, TestOk) {

  std::unordered_map<Parameters,
                     std::pair<unsigned int, std::vector<std::string>>>
      parameters;
  Parameters p{3, 5, 100, "fizz", "buzz"};
  parameters[p] = std::make_pair(1, fizzbuzz(3, 5, 100, "fizz", "buzz"));
  ASSERT_EQ(parameters[p].second == fizzbuzz(3, 5, 100, "fizz", "buzz"), true);
}

TEST(HelpersTest, TestKo) {

  std::unordered_map<Parameters,
                     std::pair<unsigned int, std::vector<std::string>>>
      parameters;
  Parameters p{3, 5, 100, "fizz", ""};
  parameters[p] = std::make_pair(1, fizzbuzz(3, 5, 100, "fizz", "buzz"));
  ASSERT_NE(parameters[p].second == fizzbuzz(3, 5, 100, "fizz", ""), true);
}

TEST(HelpersFmtErrTest, Ok) {
  auto f = []() -> std::string {
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("err_msg");
    writer.String("invalid query parameters");
    writer.EndObject();

    return s.GetString();
  };

  ASSERT_EQ(fmt_err("invalid query parameters"), f());
}

TEST(HelpersMaxHitsTest, Ok) {
  auto f = []() -> std::string {
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("max_hits");
    writer.Uint(100);
    writer.EndObject();

    return s.GetString();
  };

  ASSERT_EQ(f(), fmt_max_hits(100));
}

TEST(HelpersFmtArrayTest, Ok) {
  std::vector<std::string> values(1, "100");
  auto f = [&values]() -> std::string {
    StringBuffer s;
    Writer<StringBuffer> writer(s);

    writer.StartObject();
    writer.Key("values");
    writer.StartArray();
    for (std::vector<std::string>::iterator it = values.begin();
         it != values.end(); ++it)
      writer.String(it->c_str());
    writer.EndArray();
    writer.EndObject();

    return s.GetString();
  };

  ASSERT_EQ(f(), fmt_arr(values));
}

TEST(HelpersCmpTest, Ok) {
  int int1 = 3, int2 = 5, limit = 100;
  ASSERT_EQ(f(int1, 0), false);
  ASSERT_EQ(f(int2, 0), false);
  ASSERT_EQ(f(limit, 0), false);
  ASSERT_EQ(f(int2, int1), false);
  ASSERT_EQ(f(limit, int2), false);
}

TEST(HelpersHashTest, Ok) {
  int int1 = 3, int2 = 5, limit = 100;
  std::string str1 = "fizz";
  std::string str2 = "buzz";
  std::size_t res = 0;
  hash_combine(res, int1);
  hash_combine(res, int2);
  hash_combine(res, limit);
  hash_combine(res, str1);
  hash_combine(res, str2);
  ASSERT_NE(res, 0);
}