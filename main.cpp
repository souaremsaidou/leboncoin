//System Includes
#include <thread>
#include <map>
#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>

#include <csignal>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>

//Project Includes

//External Includes
//#include <catch.hpp>
#include <corvusoft/restbed/uri.hpp>
#include <corvusoft/restbed/byte.hpp>
#include <corvusoft/restbed/request.hpp>
#include <corvusoft/restbed/session.hpp>
#include <corvusoft/restbed/status_code.hpp>
#include <corvusoft/restbed/resource.hpp>
#include <corvusoft/restbed/service.hpp>
#include <corvusoft/restbed/settings.hpp>
#include <pqxx/pqxx>

//System Namespaces
using std::multimap;
using std::string;
using std::thread;

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
using std::chrono::seconds;
using namespace pqxx;

// global
// mutex

// resource
class FizzbuzzResource : public restbed::Resource
{
public:
	FizzbuzzResource()
	{
		this->set_path("/fizzbuzz/");
		this->set_method_handler("GET",
								 std::bind(&FizzbuzzResource::GET_method_handler, this,
										   std::placeholders::_1));
	}

	virtual ~FizzbuzzResource()
	{
	}

	void GET_method_handler(const std::shared_ptr<restbed::Session> session)
	{
		const auto request = session->get_request();

		// Getting the query params
		const int32_t int1 = request->get_query_parameter("int1", 0);
		const int32_t int2 = request->get_query_parameter("int2", 0);
		const int32_t limit = request->get_query_parameter("limit", 0);
		const std::string str1 = request->get_query_parameter("str1", "");
		const std::string str2 = request->get_query_parameter("str2", "");
		
		if (limit == 0 /**/)
		{
			session->close(400, "Invalid limit supplied", {{"Connection", "close"}});
			return;
		}


		// Change the value of this variable to the appropriate response before sending the response
		int status_code = 200;

		/**
		 * Process the received information here
		 */

		if (status_code == 200)
		{
			session->close(200, "successful operation", {{"Connection", "close"}});
			return;
		}
		if (status_code == 400)
		{
			session->close(400, "Invalid username/password supplied", {{"Connection", "close"}});
			return;
		}
	}
};

//
void ready_handler(Service &)
{
	fprintf(stderr, "Service PID is '%i'.\n", getpid());
}

void get_method_handler(const shared_ptr<Session> session)
{
	session->close(OK, "Hello, World!", {{"Content-Length", "13"}});
}

class Server
{
	shared_ptr<Settings> _settings;
	Service _service;
	shared_ptr<thread> _worker = nullptr;

public:
	Server()
	{
		_worker = make_shared<thread>([=]() {
			start();
		});
	}

	void start()
	{
		cout << "Starting server...\n";
		// add resources
		auto fizzbuzzResource = make_shared<FizzbuzzResource>();

		_settings = make_shared<Settings>();
		// read variable argument list
		_settings->set_port(1984);
		_settings->set_default_header("Connection", "close");

		_service.publish(fizzbuzzResource);

		_service.set_ready_handler(ready_handler);
		_service.set_signal_handler(SIGINT, [this](const int signo) {
			fprintf(stderr, "Received SIGINT signal number '%i'.\n", signo);
			_service.stop();
		});
		_service.set_signal_handler(SIGTERM, [this](const int signo) {
			fprintf(stderr, "Received SIGTERM signal number '%i'.\n", signo);
			_service.stop();
		});

		_service.start(_settings);
	}

	void stop()
	{
		cout << "Stopping server\n";
		_service.stop();
	}

	~Server()
	{
		_worker->join();
	}
};

void db()
{
	try
	{
		connection C("dbname = testdb user = postgres password = cohondob \
      hostaddr = 127.0.0.1 port = 5432");
		if (C.is_open())
		{
			cout << "Opened database successfully: " << C.dbname() << endl;
		}
		else
		{
			cout << "Can't open database" << endl;
		}
		C.disconnect();
	}
	catch (const std::exception &e)
	{
		cerr << e.what() << std::endl;
	}
}

int main(const int, const char **)
{
	Server server;

	//db();
	//server.stop();
	return EXIT_SUCCESS;
}
