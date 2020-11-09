//System Includes
#include <algorithm>
#include <unordered_map>
#include <string>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <csignal>
#include <utility> // std::pair, std::make_pair
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
	int limit;
	string str1;
	string str2;

	bool operator==(const Parameters &rhs) const
	{
		return int1 == rhs.int1 && int2 == rhs.int2 && limit == rhs.limit && str1 == rhs.str1 && str2 == rhs.str2;
	}

	friend ostream &operator<<(ostream &os, const Parameters &p);
};

ostream &operator<<(ostream &os, const Parameters &p)
{
	os << "[int1=" << p.int1 << ",int2=" << p.int2 << ",str1=" << p.str1 << ",str2=" << p.str2 << "]";
	return os;
}

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
		hash_combine(res, p.limit);
		hash_combine(res, p.str1);
		hash_combine(res, p.str2);
		return res;
	}
};
} // namespace std

std::mutex mtx;
std::unordered_map<Parameters, std::pair<int, std::vector<string>>> parameters;

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
		// typedef
		using query_parameters_type = std::multimap<std::string, std::string>;
		using value_type = std::pair<int, std::vector<string>>;
		using result_type = std::unordered_map<Parameters, value_type>::iterator;
		
		const auto request = session->get_request();
		
		// Getting the query params
		query_parameters_type query_parameters = request->get_query_parameters();
		if (query_parameters.size())
		{
			session->close(400, "no query parameters allowed", {{"Connection", "close"}});
			return;
		}

		result_type result;
		{
			std::lock_guard<std::mutex> lck(mtx);
			result = std::max_element(parameters.begin(), parameters.end(),
												  [](const std::pair<Parameters, value_type> &a, const std::pair<Parameters, value_type> &b) -> bool {
													  return a.second.second < b.second.second;
												  });

			// check if iterator is valid
			if (result != parameters.end())
			{
				// print maximum hits
				cout << "the most frequent request has been " << result->first << " -- hits: " << result->second.first << endl;
				session->close(200, "successful operation", {{"Connection", "close"}, { "Content-Type", "application/json" }});
			}
			else
			{
				session->close(200, "successful operation empty result", {{"Connection", "close"}, { "Content-Type", "application/json" }});
			}
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
		const std::string str1 = request->get_query_parameter("str1");
		const std::string str2 = request->get_query_parameter("str2");

		/**
		 * Process the received information here
		 */

		// Check query parameters validity
		if (f(int1, 0) || int1 == 0)
		{
			session->close(400, "Invalid query parameter int1 supplied, int1 must be greater than zero", {{"Connection", "close"}});
			return;
		}

		if (f(int2, 0) || int2 == 0)
		{
			session->close(400, "Invalid query parameter int2 supplied, int2 must be greater than zero", {{"Connection", "close"}});
			return;
		}

		if (f(int2, int1))
		{
			session->close(400, "Invalid query parameter int2 supplied, int2 must be greather than int1", {{"Connection", "close"}});
			return;
		}

		if (f(limit, 0) || limit == 0)
		{
			session->close(400, "Invalid query parameter limit supplied, limit must be greater than zero", {{"Connection", "close"}});
			return;
		}

		if (f(limit, int2))
		{
			session->close(400, "Invalid query parameter limit supplied, limit must be greater than int2", {{"Connection", "close"}});
			return;
		}

		if (str1.empty())
		{
			session->close(400, "Invalid query parameter str1 supplied, str1 must not be empty", {{"Connection", "close"}});
			return;
		}

		if (str2.empty())
		{
			session->close(400, "Invalid query parameter str2 supplied, str2 must not be empty", {{"Connection", "close"}});
			return;
		}

		// register hits
		{
			std::lock_guard<std::mutex> lck(mtx);
			vector<string> fizzbuzz;
			fizzbuzz.reserve(limit);
			Parameters p{int1, int2, limit, str1, str2};
			if (parameters.count(p) > 0) // it parameters already used then increment hits
			{
				parameters[p].first++;
				fizzbuzz = parameters[p].second;
			}
			else
			{
				// compute fizzbuzz here
				for (int i=0; i<limit; ++i) {
					fizzbuzz.push_back("1");
			  	}
				parameters[p] = std::make_pair(1, fizzbuzz);
			}

			cout << "hits: " << parameters[p].first << endl;
			cout << "Returns a list of strings with numbers from 1 to limit: " << parameters[p].first;
			for (std::vector<string>::iterator it = parameters[p].second.begin(); it != parameters[p].second.end(); ++it)
				cout << ' ' << *it;
			cout << endl;
			
			// return fizzbuzz as json list of string
			session->close(200, "successful operation fizzbuzz", {{"Connection", "close"}, { "Content-Type", "application/json" }});
		}
	}
};

//
void ready_handler(Service &)
{
	fprintf(stderr, "Service PID is '%i'.\n", getpid());
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
