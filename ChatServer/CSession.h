#pragma once
#include <boost/asio.hpp>
#include "const.h"
#include "MsgNode.h"
#include "LogicSystem.h"

using namespace std;
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class CServer;//声明CServer，避免互引用
class LogicSystem;

class CSession: public std::enable_shared_from_this<CSession>
{
public:
    CSession(boost::asio::io_context &io_context,CServer* server);
    ~CSession();
    tcp::socket &GetSocket();
    std::string &Getuuid();
    void Start();
    void Send(char *msg,short max_length,short msgid);
    void Send(std::string msg,short msgid);
    std::shared_ptr<CSession> SharedSelf();
    void Close();
    void AsyncReadBody(int total_len);//读取包体
    void AsyncReadHead(int total_len);//读取头部信息
private:

    void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code& , std::size_t)> handler);//读取完整长度
    void asyncReadLen(std::size_t read_len, std::size_t total_len, std::function<void(const boost::system::error_code&, std::size_t)> handler);//读取指定字节数
    void HandleWrite(const boost::system::error_code &error,std::shared_ptr<CSession> shared_self);//用于投送消息
    tcp::socket _socket;
    std::string _uuid;
    char _data[MAX_LENGTH];
    CServer *_server;
    bool _b_close;
    bool _b_head_parse;
    std::queue<shared_ptr<SendNode>> _send_que;
    std::mutex _send_lock;

    //收到的消息结构
    std::shared_ptr<RecvNode> _recv_msg_node;
    //收到的头部结构
    std::shared_ptr<MsgNode> _recv_head_node;




};


class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);
private:
	shared_ptr<CSession> _session;
	shared_ptr<RecvNode> _recvnode;
};