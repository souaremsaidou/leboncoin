// System Includes

// Project Includes
#include "helpers.hpp"

// External Includes
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <boost/program_options.hpp> // boost::program_options

// System Namespaces
using std::string;
using namespace std;

// Project Namespaces

// External Namespaces
using namespace rapidjson;

//
std::string fmt_err(const std::string &err_msg) {
  StringBuffer s;
  Writer<StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("err_msg");
  writer.String(err_msg.c_str());
  writer.EndObject();

  return s.GetString();
}

//
std::string fmt_max_hits(int max_hits) {
  StringBuffer s;
  Writer<StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("max_hits");
  writer.Uint(max_hits);
  writer.EndObject();

  return s.GetString();
}

//
std::string fmt_arr(std::vector<std::string> &values) {
  StringBuffer s;
  Writer<StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("values");
  writer.StartArray();
  for (std::vector<string>::iterator it = values.begin(); it != values.end();
       ++it)
    writer.String(it->c_str());
  writer.EndArray();
  writer.EndObject();

  return s.GetString();
}

//
bool Parameters::operator==(const Parameters &rhs) const {
  return int1 == rhs.int1 && int2 == rhs.int2 && limit == rhs.limit &&
         str1 == rhs.str1 && str2 == rhs.str2;
}

// debug purpose only
ostream &operator<<(ostream &os, const Parameters &p) {
  os << "(int1=" << p.int1 << ",int2=" << p.int2 << ",str1=" << p.str1
     << ",str2=" << p.str2 << ")";
  return os;
}