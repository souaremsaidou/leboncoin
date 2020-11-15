// System Includes
#include <thread>

// Project Includes
#include "api.hpp"

// External Includes
#include <corvusoft/restbed/byte.hpp>
#include <corvusoft/restbed/request.hpp>
#include <corvusoft/restbed/resource.hpp>
#include <corvusoft/restbed/service.hpp>
#include <corvusoft/restbed/session.hpp>
#include <corvusoft/restbed/settings.hpp>
#include <corvusoft/restbed/status_code.hpp>
#include <corvusoft/restbed/uri.hpp>
#include <pqxx/pqxx>

// System Namespaces
using namespace std;
using std::string;
using std::thread;

// Project Namespaces

// External Namespaces
using restbed::Bytes;
using restbed::OK;
using restbed::Request;
using restbed::Resource;
using restbed::Service;
using restbed::Session;
using restbed::Settings;
using restbed::Uri;

std::mutex mtx;
std::unordered_map<Parameters, std::pair<unsigned int, std::vector<string>>>
    parameters;

//
StatisticsResource::StatisticsResource() {
  this->set_path("/statistics/");
  this->set_method_handler("GET",
                           std::bind(&StatisticsResource::GET_method_handler,
                                     this, std::placeholders::_1));
}

StatisticsResource::~StatisticsResource() {}

void StatisticsResource::GET_method_handler(
    const std::shared_ptr<restbed::Session> session) {
  // typedef
  using query_parameters_type = std::multimap<std::string, std::string>;
  using value_type = std::pair<unsigned int, std::vector<string>>;
  using result_type = std::unordered_map<Parameters, value_type>::iterator;

  const auto request = session->get_request();

  // Getting the query params
  query_parameters_type query_parameters = request->get_query_parameters();
  if (query_parameters.size()) {
    session->close(
        400, fmt_err("no query parameters allowed"),
        {{"Connection", "close"}, {"Content-Type", "application/json"}});
    return;
  }

  result_type result;
  {
    std::lock_guard<std::mutex> lck(mtx);
    result = std::max_element(
        parameters.begin(), parameters.end(),
        [](const std::pair<Parameters, value_type> &a,
           const std::pair<Parameters, value_type> &b) -> bool {
          return a.second.second < b.second.second;
        });

    // check if iterator is valid
    if (result != parameters.end()) {
      session->close(
          200, fmt_max_hits(result->second.first),
          {{"Connection", "close"}, {"Content-Type", "application/json"}});
    } else {
      session->close(
          200, fmt_err("successful operation, but empty result"),
          {{"Connection", "close"}, {"Content-Type", "application/json"}});
    }
  }
}

FizzbuzzResource::FizzbuzzResource() {
  this->set_path("/fizzbuzz/");
  this->set_method_handler("GET",
                           std::bind(&FizzbuzzResource::GET_method_handler,
                                     this, std::placeholders::_1));
}

FizzbuzzResource::~FizzbuzzResource() {}

void FizzbuzzResource::GET_method_handler(
    const std::shared_ptr<restbed::Session> session) {
  const auto request = session->get_request();

  // Getting the query params
  const int int1 = request->get_query_parameter("int1", 0);
  const int int2 = request->get_query_parameter("int2", 0);
  const int limit = request->get_query_parameter("limit", 0);
  const std::string str1 = request->get_query_parameter("str1");
  const std::string str2 = request->get_query_parameter("str2");

  // Check query parameters validity
  if (f(int1, 0) || int1 == 0) {
    session->close(
        400, fmt_err("Invalid query parameter int1 supplied, int1 must be "
                     "an integer greater than zero"),
        {{"Connection", "close"}});
    return;
  }

  if (f(int2, 0) || int2 == 0) {
    session->close(
        400, fmt_err("Invalid query parameter int2 supplied, int2 must be "
                     "an integer greater than zero"),
        {{"Connection", "close"}});
    return;
  }

  if (f(int2, int1) || int2 == int1) {
    session->close(
        400, fmt_err("Invalid query parameter int2 supplied, int2 must be "
                     "an integer greather than int1"),
        {{"Connection", "close"}});
    return;
  }

  if (f(limit, 0) || limit == 0) {
    session->close(400,
                   fmt_err("Invalid query parameter limit supplied, limit must "
                           "be an integer greater than zero"),
                   {{"Connection", "close"}});
    return;
  }

  if (f(limit, int2)) {
    session->close(400,
                   fmt_err("Invalid query parameter limit supplied, limit must "
                           "be an integer greater than int2"),
                   {{"Connection", "close"}});
    return;
  }

  if (str1.empty()) {
    session->close(
        400,
        fmt_err(
            "Invalid query parameter str1 supplied, str1 must not be empty"),
        {{"Connection", "close"}});
    return;
  }

  if (str2.empty()) {
    session->close(
        400,
        fmt_err(
            "Invalid query parameter str2 supplied, str2 must not be empty"),
        {{"Connection", "close"}});
    return;
  }

  Parameters p{int1, int2, limit, str1, str2};
  {
    std::lock_guard<std::mutex> lck(mtx);
    if (parameters.count(p) > 0) {
      parameters[p].first++;
    } else {
      parameters[p] =
          std::make_pair(1, fizzbuzz(int1, int2, limit, str1, str2));
    }
  }

  // return fizzbuzz as json list of string
  session->close(
      200, fmt_arr(parameters[p].second),
      {{"Connection", "close"}, {"Content-Type", "application/json"}});
}