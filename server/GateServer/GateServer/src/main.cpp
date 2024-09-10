#include "CServer.h"

//#include <iostream>
//#include <json/json.h>
//#include <json/value.h>
//#include <json/reader.h>

void test_json() {
    Json::Value root;
    root["id"] = 1001;
    root["data"] = "hello world";
    std::string request = root.toStyledString();
    std::cout << "request is " << request << std::endl;

    Json::Value root2;
    Json::Reader reader;
    reader.parse(request, root2);
    std::cout << "msg id is " << root2["id"] << " msg is " << root2["data"] << std::endl;
}

int main()
{
    // test_json();
    try
    {
        unsigned short port = static_cast<unsigned short>(8080);
        asio::io_context ioc{1};
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code &error, int signal_number)
                           {
                               if (error)
                               {
                                   return;
                               }
                               ioc.stop();
                           });
        std::make_shared<CServer>(ioc, port)->Start();
        ioc.run();
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
