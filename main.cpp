//System Includes
#include <map>
#include <string>
#include <memory>
#include <cstdlib>

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
using std::string;
using std::multimap;

//Project Namespaces
using restbed::Uri;
using restbed::Bytes;
using restbed::Request;
using restbed::Session;
using restbed::OK;
using restbed::Resource;
using restbed::Settings;
using restbed::Service;

//External Namespaces
using namespace std;


void get_method_handler( const shared_ptr< Session > session )
{
    session->close( OK, "Hello, World!", { { "Content-Length", "13" } } );
}

int main( const int, const char** )
{
    auto resource = make_shared< Resource >( );
    resource->set_paths( { "/messages", "/queues/{id: [0-9]*}/messages" } );
    resource->set_method_handler( "GET", get_method_handler );
    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    settings->set_default_header( "Connection", "close" );
    
    Service service;
    service.publish( resource );
    service.start( settings );
    
    return EXIT_SUCCESS;
}

