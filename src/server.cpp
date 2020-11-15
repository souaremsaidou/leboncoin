// System Includes
#include <algorithm>
#include <memory>

// Project Includes
#include "api.hpp"
#include "server.hpp"

// External Includes
#include <corvusoft/restbed/resource.hpp>
#include <corvusoft/restbed/service.hpp>
#include <corvusoft/restbed/settings.hpp>

// System Namespaces
using namespace std;
using std::string;
using std::thread;

// Project Namespaces
using restbed::Service;
using restbed::Settings;

// External Namespaces

Server::Server(const string &host, int port) : _host(host), _port(port) {
  _worker = make_shared<thread>([=]() { start(); });
}

void Server::start() {
  cout << "Serving Fizzbuzz web server \n";

  // add resources
  auto fizzbuzzResource = make_shared<FizzbuzzResource>();
  auto statisticsResource = make_shared<StatisticsResource>();

  _settings = make_shared<Settings>();
  _settings->set_bind_address(_host);
  _settings->set_port(_port);
  _settings->set_worker_limit(4);
  _settings->set_default_header("Connection", "close");

  _service.publish(fizzbuzzResource);
  _service.publish(statisticsResource);

  _service.set_ready_handler([this](Service &service) {
    cout << "Running on http://" << _host << ":" << _port
         << "/ (Press CTRL+C to quit)\n";
  });

  auto signal_handler = [this](const int signo) {
    fprintf(stderr, "Shut the service down");
    _service.stop();
  };
  // Set a handler for the SIGINT system signal number.
  _service.set_signal_handler(SIGINT, signal_handler);
  // Set a handler for the SIGTERM system signal number.
  _service.set_signal_handler(SIGTERM, signal_handler);

  // Start the service with the supplied settings.
  _service.start(_settings);
}

void Server::stop() {
  cout << "Stopping Fizzbuzz web server" << endl;
  _service.stop();
}

Server::~Server() { _worker->join(); }
