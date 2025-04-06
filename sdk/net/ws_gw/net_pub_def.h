#ifndef NET_PUB_DEF_H
#define NET_PUB_DEF_H

// #include "boost/beast.hpp"
#include <boost/log/trivial.hpp>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#define PX_LOG_DEBUG() BOOST_LOG_TRIVIAL(debug) << "|"
#define PX_LOG_INFO() BOOST_LOG_TRIVIAL(info) << "|"
#define PX_LOG_WARN() BOOST_LOG_TRIVIAL(warning) << "|"
#define PX_LOG_ERROR() BOOST_LOG_TRIVIAL(error) << "|"

// #define PX_LOG_DEBUG()   BOOST_LOG_TRIVIAL(debug)<<BOOST_CURRENT_FUNCTION<<"|"
// #define PX_LOG_INFO()    BOOST_LOG_TRIVIAL(info)<<BOOST_CURRENT_FUNCTION<<"|"
// #define PX_LOG_WARN()    BOOST_LOG_TRIVIAL(warning)<<BOOST_CURRENT_FUNCTION<<"|"
// #define PX_LOG_ERROR()   BOOST_LOG_TRIVIAL(error)<<BOOST_CURRENT_FUNCTION<<"|"

#define PX_DEBUG PX_LOG_DEBUG
#define PX_INFO PX_LOG_INFO
#define PX_WARN PX_LOG_WARN
#define PX_ERROR PX_LOG_ERROR

// namespace http = boost::beast::http;

// typedef http::request_parser<http::empty_body> EmptyRequestParser;
// typedef http::request<http::string_body> StrRequest;
// typedef http::response<http::string_body> StrResponse;

// typedef http::request<http::buffer_body> BufferRequest;
// typedef http::response<http::buffer_body> BufferResponse;

// typedef boost::asio::io_context IoContext;
// typedef boost::asio::ip::tcp::acceptor Acceptor;
// typedef boost::asio::ip::tcp::resolver Resolver;
// typedef boost::asio::ip::tcp::resolver::results_type ResolverResult;

// typedef boost::asio::ip::tcp::endpoint Endpoint;
// typedef boost::asio::ip::tcp::socket TcpSocket;

// typedef boost::asio::steady_timer DTimer;

// typedef boost::asio::coroutine Coroutine;

#endif