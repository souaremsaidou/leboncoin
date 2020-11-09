//System Includes
#include <unordered_map>
#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <csignal>
#include <sys/types.h>
#include <unistd.h>
#include <thread> // std::thread
#include <mutex>  // std::mutex, std::lock_guard
//#include <stdexcept>      // std::logic_error
#include <chrono>
#include <functional>
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

template <class T>
inline void hash_combine(std::size_t &s, const T &v)
{
	std::hash<T> h;
	s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
}

struct Parameters
{
	int int1;
	int int2;
	// int limit;
	string str1;
	string str2;

	bool operator==(const Parameters &rhs) const
	{
		return int1 == rhs.int1 && int2 == rhs.int2 && /*limit == rhs.limit &&*/ str1 == rhs.str1 && str2 == rhs.str2;
	}
};

namespace std
{
template <>
struct hash<Parameters>
{
	std::size_t operator()(const Parameters &p) const
	{
		std::size_t res = 0;
		hash_combine(res, p.int1);
		hash_combine(res, p.int2);
		hash_combine(res, p.str1);
		hash_combine(res, p.str2);
		return res;
	}
};
} // namespace std

template <class T>
class MyHash;

template <>
struct MyHash<Parameters>
{
	std::size_t operator()(Parameters const &p) const
	{
		std::size_t res = 0;
		hash_combine(res, p.int1);
		hash_combine(res, p.int2);
		hash_combine(res, p.str1);
		hash_combine(res, p.str2);
		return res;
	}
};

std::mutex mtx;
std::unordered_map<Parameters, int> parameters;

// std::less
template <typename A, typename B, typename U = std::less<>>
bool f(A a, B b, U u = U())
{
	return u(a, b);
}

// resource
class StatisticsResource : public restbed::Resource
{
public:
	StatisticsResource()
	{
		this->set_path("/statistics/");
		this->set_method_handler("GET",
								 std::bind(&StatisticsResource::GET_method_handler, this,
										   std::placeholders::_1));
	}

	virtual ~StatisticsResource()
	{
	}

	void GET_method_handler(const std::shared_ptr<restbed::Session> session)
	{
		{
			std::lock_guard<std::mutex> lck(mtx);
			//std::cout << "statistics parameters contains:";
			//for (auto& x: parameters)
			//	std::cout << " [" << x.first << ':' << x.second << ']';
			//std::cout << '\n';
			cout << "statistics\t" << ::this_thread::get_id() << "\t" << parameters.size() << endl;
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
		const int int1 = request->get_query_parameter("int1", 0);
		const int int2 = request->get_query_parameter("int2", 0);
		const int limit = request->get_query_parameter("limit", 0);
		const std::string str1 = request->get_query_parameter("str1", "");
		const std::string str2 = request->get_query_parameter("str2", "");

		{
			std::lock_guard<std::mutex> lck(mtx);
			cout << "params" << int1 << "\t" << int2 << "\t" << str1 << "\t" << str2 << endl;
			parameters.emplace(Parameters{int1, int2, str1, str2}, 0);
			cout << "fizzbuzz\t" << ::this_thread::get_id() << "\t" << parameters.size() << endl;
		}

		// check param type

		// check param is in valid range
		if (f(int1, 0) || int1 == 0)
		{
			session->close(400, "Invalid int1 supplied, int1 must be greater than zero", {{"Connection", "close"}});
			return;
		}

		if (f(int2, 0) || int2 == 0)
		{
			session->close(400, "Invalid int2 supplied, int2 must be greater than zero", {{"Connection", "close"}});
			return;
		}

		if (f(int2, int1))
		{
			session->close(400, "Invalid parameters int2 supplied, int2 must be greather than int1", {{"Connection", "close"}});
			return;
		}

		if (f(limit, 0) || limit == 0)
		{
			session->close(400, "Invalid limit supplied, limit must be greater than zero", {{"Connection", "close"}});
			return;
		}

		if (f(limit, int2))
		{
			session->close(400, "Invalid parameters supplied", {{"Connection", "close"}});
			return;
		} // and more cmp

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
		auto statisticsResource = make_shared<StatisticsResource>();

		_settings = make_shared<Settings>();
		// read variable argument list
		_settings->set_port(1984);
		_settings->set_worker_limit(4);
		_settings->set_default_header("Connection", "close");

		_service.publish(fizzbuzzResource);
		_service.publish(statisticsResource);

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
