#ifndef CONST_H
#define CONST_H

#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>
#include <queue>
#include <atomic>
#include <hiredis/hiredis.h>
#include <memory>



enum ErrorCodes{
    Success = 0,
    Error_Json = 1001,//Json解析错误
    RPCFailed = 1002,//RPC请求错误
};

#endif // CONST_H