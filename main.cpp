//System Includes
#include <map>
#include <string>
#include <memory>
#include <cstdlib>
#include <csignal>
#include <iostream>

//Project Includes
#include <corvusoft/restbed/uri.hpp>
#include <corvusoft/restbed/byte.hpp>
#include <corvusoft/restbed/request.hpp>
#include <corvusoft/restbed/session.hpp>
#include <corvusoft/restbed/status_code.hpp>
#include <corvusoft/restbed/resource.hpp>
#include <corvusoft/restbed/service.hpp>
#include <corvusoft/restbed/settings.hpp>

//External Includes
//#include <catch.hpp>

//System Namespaces
using std::multimap;
using std::string;

//Project Namespaces
using restbed::Bytes;
using restbed::OK;
using restbed::Request;
using restbed::Resource;
using restbed::Service;
using restbed::Session;
using restbed::Settings;
using restbed::Uri;

//External Namespaces
using namespace std;

void get_method_handler(const shared_ptr<Session> session)
{
	session->close(OK, "Hello, World!", {{"Content-Length", "13"}});
}

class Server
{
	shared_ptr<Settings> _settings;
	Service _service;

public:
	Server()
	{
		cout << "Starting server...\n";
		auto resource = make_shared<Resource>();
		resource->set_paths({"/messages", "/queues/{id: [0-9]*}/messages"});
		resource->set_method_handler("GET", get_method_handler);

		_settings = make_shared<Settings>();
		_settings->set_port(1984);
		_settings->set_default_header("Connection", "close");

		_service.publish(resource);
	}

	void start()
	{
		_service.set_signal_handler( SIGINT, [this](const int signo) {
			cout << "Received SIGINT signal number " << signo << '\n';
			_service.stop();
		    });

		_service.start(_settings);
	}

	~Server()
	{
		cout << "Stopping server\n";
	}
};

int main(const int, const char **)
{
	Server server;
	server.start();
	return EXIT_SUCCESS;
}
