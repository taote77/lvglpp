#include "httplib.h"
#include <iostream>

int main(int argc, char *argv[])
{
    std::cout << "Hello, World!" << std::endl;

    // Create a server instance

    httplib::Server svr;

    // file index
    svr.set_base_dir("/home/shane/workspace/lvglpp/demo/remote_ctrl/index/");

    // Set up a GET route
    svr.Get("/", [](const httplib::Request &req, httplib::Response &res) { res.set_content("Hello, World!", "text/plain"); });
    // Start the server on port 8080
    if (svr.listen("0.0.0.0", 8080))
    {
        std::cout << "Server is running on port 8080" << std::endl;
    }

    return 0;
}