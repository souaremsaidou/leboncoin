#pragma once

// System Includes
#include <mutex> // std::mutex, std::lock_guard
#include <unordered_map>

// Project Includes
#include "fizzbuzz.hpp"
#include "helpers.hpp"

// External Includes
#include <corvusoft/restbed/resource.hpp>

//
extern std::mutex mtx;
// shared data
extern std::unordered_map<Parameters,
                          std::pair<unsigned int, std::vector<std::string>>>
    parameters;

// resource
class StatisticsResource : public restbed::Resource {
public:
  StatisticsResource();

  virtual ~StatisticsResource();

  void GET_method_handler(const std::shared_ptr<restbed::Session> session);
};

class FizzbuzzResource : public restbed::Resource {
public:
  FizzbuzzResource();

  virtual ~FizzbuzzResource();

  void GET_method_handler(const std::shared_ptr<restbed::Session> session);
};