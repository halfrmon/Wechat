#include "CServer.h"


#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& io_context, short port):_io_context(io_context), _port(port),
    _acceptor(io_context, tcp::endpoint(tcp::v4(),port))//参数列表初始化
{
    std::cout<<"Server start success,listen on port"<<std::endl;
    StartAccept();
}
CServer::~CServer(){

}
void CServer::StartAccept() {
    auto &io_context = AsioIOServicePool::GetInstance()->GetIOService();
    shared_ptr<CSession> new_session = make_shared<CSession>(io_context, this);
    _acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session, placeholders::_1));
}

void CServer::HandleAccept(shared_ptr<CSession>new_session,const boost::system::error_code &error)
{
    if(!error){
        new_session->Start();
        lock_guard<mutex> lock(_mutex);
        _sessions.insert(make_pair(new_session->Getuuid(),new_session));
    }
    else{
        std::cout<<"session accept failed,error is : "<<error.value()<<std::endl;
    }
    StartAccept();
}
void CServer::ClearSession(std::string _uuid){
    std::lock_guard<std::mutex> lock(_mutex);
    _sessions.erase(_uuid);
}
