#pragma once
#include "Singleton.h"
#include<functional>
#include <map>
#include "const.h"
class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
class LogicSystem:public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    bool HandleGet(std::string,std::shared_ptr<HttpConnection>);//处理get类型的请求，并且为了实现并发，还绑定了一个连接的智能指针，来实现处理多个连接的get方法解析
    bool HandlePost(std::string,std::shared_ptr<HttpConnection>);//处理post请求
    void RegGet(std::string,HttpHandler handler);//注册get类型的请求
    void RegPost(std::string,HttpHandler handler);//注册post类型的请求
private:
    LogicSystem();
    std::map<std::string,HttpHandler> _post_handlers;
    std::map<std::string,HttpHandler> _get_handlers;
};