// System Includes
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex> // std::mutex, std::lock_guard
#include <sstream>
#include <string>
#include <sys/types.h>
#include <thread> // std::thread
#include <unistd.h>
#include <unordered_map>
#include <utility> // std::pair, std::make_pair
//#include <stdexcept>      // std::logic_error
#include <chrono>
#include <fstream>
#include <functional>
#include <iterator>

// Project Includes
#include "fizzbuzz.hpp"

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
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
using namespace rapidjson;

#include <boost/program_options.hpp> // boost::program_options

// System Namespaces
using std::multimap;
using std::string;
using std::thread;
using std::chrono::seconds;

// Project Namespaces
using restbed::Bytes;
using restbed::OK;
using restbed::Request;
using restbed::Resource;
using restbed::Service;
using restbed::Session;
using restbed::Settings;
using restbed::Uri;

// External Namespaces
using namespace std;
using namespace pqxx;
namespace po = boost::program_options;

// JSON
std::string fmt_err(const std::string &err_msg) {
  StringBuffer s;
  Writer<StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("err_msg");
  writer.String(err_msg.c_str());
  writer.EndObject();

  return s.GetString();
}

std::string fmt_max_hits(int max_hits) {
  StringBuffer s;
  Writer<StringBuffer> writer(s);

  writer.StartObject();
  writer.Key("max_hits");
  writer.Uint(max_hits);
  writer.EndObject();

  return s.GetString();
}

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

struct Parameters {
  int int1;
  int int2;
  int limit;
  string str1;
  string str2;

  bool operator==(const Parameters &rhs) const {
    return int1 == rhs.int1 && int2 == rhs.int2 && limit == rhs.limit &&
           str1 == rhs.str1 && str2 == rhs.str2;
  }

  // Parameters friend functions
  friend ostream &operator<<(ostream &os, const Parameters &p);
};

// log putpose
ostream &operator<<(ostream &os, const Parameters &p) {
  os << "[int1=" << p.int1 << ",int2=" << p.int2 << ",str1=" << p.str1
     << ",str2=" << p.str2 << "]";
  return os;
}

/*
 * create an std::unordered_map with user defined key
 */
template <class T> inline void hash_combine(std::size_t &s, const T &v) {
  std::hash<T> h;
  s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

namespace std {
template <> struct hash<Parameters> {
  std::size_t operator()(const Parameters &p) const {
    std::size_t res = 0;
    hash_combine(res, p.int1);
    hash_combine(res, p.int2);
    hash_combine(res, p.limit);
    hash_combine(res, p.str1);
    hash_combine(res, p.str2);
    return res;
  }
};
} // namespace std
std::mutex mtx;
std::unordered_map<Parameters, std::pair<unsigned int, std::vector<string>>> parameters;

// std::less
template <typename A, typename B, typename U = std::less<>>
bool f(A a, B b, U u = U()) {
  return u(a, b);
}

// resource
class StatisticsResource : public restbed::Resource {
public:
  StatisticsResource() {
    this->set_path("/statistics/");
    this->set_method_handler("GET",
                             std::bind(&StatisticsResource::GET_method_handler,
                                       this, std::placeholders::_1));
  }

  virtual ~StatisticsResource() {}

  void GET_method_handler(const std::shared_ptr<restbed::Session> session) {
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
            200, fmt_err("successful operation empty result"),
            {{"Connection", "close"}, {"Content-Type", "application/json"}});
      }
    }
  }
};

class FizzbuzzResource : public restbed::Resource {
public:
  FizzbuzzResource() {
    this->set_path("/fizzbuzz/");
    this->set_method_handler("GET",
                             std::bind(&FizzbuzzResource::GET_method_handler,
                                       this, std::placeholders::_1));
  }

  virtual ~FizzbuzzResource() {}

  void GET_method_handler(const std::shared_ptr<restbed::Session> session) {
    const auto request = session->get_request();

    // Getting the query params
    const int int1 = request->get_query_parameter("int1", 0);
    const int int2 = request->get_query_parameter("int2", 0);
    const int limit = request->get_query_parameter("limit", 0);
    const std::string str1 = request->get_query_parameter("str1");
    const std::string str2 = request->get_query_parameter("str2");

    /**
     * Process the received information here
     */

    // Check query parameters validity
    if (f(int1, 0) || int1 == 0) {
      session->close(
          400, fmt_err("Invalid query parameter int1 supplied, int1 must be "
                       "greater than zero)"),
          {{"Connection", "close"}});
      return;
    }

    if (f(int2, 0) || int2 == 0) {
      session->close(
          400, fmt_err("Invalid query parameter int2 supplied, int2 must be "
                       "greater than zero"),
          {{"Connection", "close"}});
      return;
    }

    if (f(int2, int1)) {
      session->close(
          400, fmt_err("Invalid query parameter int2 supplied, int2 must be "
                       "greather than int1"),
          {{"Connection", "close"}});
      return;
    }

    if (f(limit, 0) || limit == 0) {
      session->close(
          400, fmt_err("Invalid query parameter limit supplied, limit must "
                       "be greater than zero"),
          {{"Connection", "close"}});
      return;
    }

    if (f(limit, int2)) {
      session->close(
          400, fmt_err("Invalid query parameter limit supplied, limit must "
                       "be greater than int2"),
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
      if (parameters.count(p) >
          0) // it parameters already used then increment hits
      {
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
};

[[maybe_unused]] void ready_handler(Service &) {
  fprintf(stderr, "Service PID is '%i'.\n", getpid());
}

class Server {
  // Represents service configuration.
  shared_ptr<Settings> _settings;

  // The service is responsible for managing the publicly available
  // RESTfulresources,HTTP compliance, scheduling of the socket data and
  // insuring incoming requests are processed in a timely fashion.
  Service _service;

  //
  shared_ptr<thread> _worker = nullptr;

  // the local network interface card address to attach the service.
  const std::string &_host;

  // the network port which the service should listen for incoming HTTP
  // requests.
  const int _port;

public:
  Server(const std::string &host, int port) : _host(host), _port(port) {
    _worker = make_shared<thread>([=]() { start(); });
  }

  void start() {
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

  void stop() {
    cout << "Stopping Fizzbuzz web server" << endl;
    _service.stop();
  }

  ~Server() { _worker->join(); }
};

void db() {
  try {
    connection C("dbname = testdb user = postgres password = cohondob \
      hostaddr = 127.0.0.1 port = 5432");
    if (C.is_open()) {
      cout << "Opened database successfully: " << C.dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
    }
    C.disconnect();
  } catch (const std::exception &e) {
    cerr << e.what() << std::endl;
  }
}

int main(int ac, char *av[]) {
  int port;
  string host;

  // Declare the supported options.
  try {
    po::options_description generic("Generic options");
    generic.add_options()("help", "produce help message")(
        "version,v", "print version string");

    po::options_description config("Server configuration");
    config.add_options()("host",
                         po::value<string>(&host)->default_value("127.0.0.1"),
                         "set the server host address")(
        "port", po::value<int>(&port)->default_value(1984),
        "set the server port number");

    po::options_description hidden("db configuration");
    hidden.add_options()("dbname", po::value<string>(),
                         "set dbname")("user", po::value<string>(), "set user")(
        "password", po::value<string>(),
        "set password")("hostaddr", po::value<string>(), "set host address")(
        "hostport", po::value<string>(), "set host port");

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    po::options_description config_file_options;
    config_file_options.add(hidden);

    po::options_description visible;
    visible.add(generic).add(config);

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, cmdline_options), vm);

    ifstream ifs("db.cfg");
    store(parse_config_file(ifs, config_file_options), vm);
    notify(vm);

    if (vm.count("help")) {
      cout << "Usage: " << av[0] << " --host address --port port" << std::endl;
      cout << visible << std::endl;
      return 0;
    }

    if (vm.count("version")) {
      cout << "Fizzbuzz web server, version 1.0\n";
      return 0;
    }

    if (!vm.count("dbname") && !vm.count("user") && !vm.count("password") &&
        !vm.count("hostaddr") && !vm.count("hostport")) {
      cout << "db config failed." << std::endl;
      return 1;
    } else {
      cout << "db config is done." << std::endl;
    }

  } catch (exception &e) {
    cerr << "error: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    cerr << "Exception of unknown type!" << std::endl;
    return 1;
  }

  Server server(host, port);

  // db();
  // server.stop();
  return EXIT_SUCCESS;
}
