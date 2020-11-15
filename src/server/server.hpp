#pragma once

// System Includes
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <memory>
#include <mutex> // std::mutex, std::lock_guard
#include <sstream>
#include <sys/types.h>
#include <thread> // std::thread
#include <unistd.h>
#include <unordered_map>
#include <utility> // std::pair, std::make_pair
//#include <stdexcept>      // std::logic_error
#include <chrono>
#include <fstream>
#include <iterator>

// Project Includes
#include "fizzbuzz.hpp"
#include "helpers.hpp"

// External Includes
#include <corvusoft/restbed/resource.hpp>
#include <corvusoft/restbed/service.hpp>
#include <corvusoft/restbed/session.hpp>

class Server {
  // Represents service configuration.
  std::shared_ptr<restbed::Settings> _settings;

  // The service is responsible for managing the publicly available
  // RESTfulresources,HTTP compliance, scheduling of the socket data and
  // insuring incoming requests are processed in a timely fashion.
  restbed::Service _service;

  //
  std::shared_ptr<std::thread> _worker = nullptr;

  // the local network interface card address to attach the service.
  const std::string &_host;

  // the network port which the service should listen for incoming HTTP
  // requests.
  const int _port;

public:
  Server(const std::string &host, int port);

  void start();

  void stop();

  ~Server();
};
