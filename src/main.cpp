// System Includes

// Project Includes
#include "server.hpp"

// External Includes
#include <boost/program_options.hpp> // boost::program_options

// System Namespaces

// Project Namespaces

// External Namespaces
using namespace std;
namespace po = boost::program_options;

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
