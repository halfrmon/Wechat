#include "CSession.h"
#include "CServer.h"
#include <iostream>
#include <sstream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "LogicSystem.h"

CSession::CSession(boost::asio::io_context &io_context,CServer* server):_socket(io_context),_server(server),_b_close(false),_b_head_parse(false)
{
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();//生成随机的uuid
    _uuid = boost::uuids::to_string(a_uuid);
    _recv_head_node = make_shared<MsgNode>(HEAD_TOTAL_LEN);
}

CSession::~CSession(){
    Close();
}

void CSession::Close(){
    _b_close = true;


}

tcp::socket& CSession::GetSocket(){
    return _socket;
}

std::string & CSession::Getuuid(){
    return _uuid;
}
void CSession::Start(){
    AsyncReadHead(HEAD_TOTAL_LEN);
}

void CSession::asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code& , std::size_t)> handler){//读取完整长度
    ::memset(_data,0,MAX_LENGTH);
    asyncReadLen(0,maxLength,handler);
}
void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len, std::function<void(const boost::system::error_code&, std::size_t)> handler){
    auto self = shared_from_this();
    _socket.async_read_some(boost::asio::buffer(_data+read_len,total_len-read_len),[read_len,total_len,handler,self]
    (const boost::system::error_code&error,size_t bytesTransfered){
        if(error){
            handler(error,read_len+bytesTransfered);
            return ; 
        }
        if(read_len+bytesTransfered>=total_len){
            handler(error,read_len+bytesTransfered);
        }
        self->asyncReadLen(read_len+bytesTransfered,total_len,handler);
    });


}



void CSession::AsyncReadHead(int total_len){
    auto self = shared_from_this();//引用计数，构造伪闭包
    asyncReadFull(HEAD_TOTAL_LEN,[self,this](const boost::system::error_code &error,std::size_t bytes_transfered){//将所有的数据读完后触发回调函数
        try
        {
            if(error)//表示出现错误
            {
                std::cout<<"handle read failed"<<std::endl;
                Close();
                _server->ClearSession(_uuid);//清除server中的session会话id
                return;
            }

            if(bytes_transfered<HEAD_TOTAL_LEN)//读取头部总长度没有读全
            {
                std::cout<<"read length not match read ["<<bytes_transfered<<"],total["<<HEAD_TOTAL_LEN<<"]"<<std::endl;
                Close();
                _server->ClearSession(_uuid);
                return ;
            }

            _recv_head_node->Clear();
            memcpy(_recv_head_node->_data,_data,bytes_transfered);//读取头部完了之后将头部数据拷贝至接收节点中

            //获取头部中的msgid数据
            short msg_id =0;
            memcpy(&msg_id,_recv_head_node->_data,HEAD_ID_LEN);//将数据中的id拷贝出来
            msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);//由于通过网络传输的，所以此时还是大端，需要改成小端

            if(msg_id>MAX_LENGTH){
                std::cout<<"msg_id invalid"<<std::endl;
                Close();
                _server->ClearSession(_uuid);
                return;
            }
            std::cout<<"msg_id is : "<<msg_id<<std::endl;
            //获取头部中msg_len数据
            short msg_len = 0;
            memcpy(&msg_len,_recv_head_node->_data+HEAD_ID_LEN,HEAD_DATA_LEN);
            msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
            if(msg_len>MAX_LENGTH){
                std::cout<<"msg_len invaild"<<std::endl;
                Close();
                _server->ClearSession(_uuid);
                return;
            }
            std::cout<<"msg_len is : "<<msg_len<<std::endl;
            _recv_msg_node = make_shared<RecvNode>(msg_len,msg_id);//通过解析的数据构造接收节点
            AsyncReadBody(msg_len);

        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
        

    });
}



void CSession::AsyncReadBody(int total_len){
    auto self = shared_from_this();
    asyncReadFull(total_len,[self,this,total_len](const boost::system::error_code&error,std::size_t bytes_transfered){
        try
        {
            if(error){
                std::cout<<"handle read failed,error is : "<<std::endl;
                Close();
                _server->ClearSession(_uuid);
                return;
            }
            if(bytes_transfered<total_len){
                std::cout<<"read length not match, read["<<bytes_transfered<<"], total is ["<<total_len<<"]"<<std::endl;
                Close();
                _server->ClearSession(_uuid);
                return;
            }
            memcpy(_recv_msg_node->_data,_data,bytes_transfered);
            _recv_msg_node->_cur_len+=bytes_transfered;
            _recv_msg_node->_data[_recv_msg_node->_total_len]='\0';
            std::cout<<"receive data is "<<_recv_msg_node->_data<<std::endl;
            //此处将消息投递到逻辑队列中
			LogicSystem::GetInstance()->PostMsgToQue(make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
			//继续监听头部接受事件
			AsyncReadHead(HEAD_TOTAL_LEN);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    });

}


void CSession::HandleWrite(const boost::system::error_code &error,std::shared_ptr<CSession> shared_self){//处理消息节点队列的推送

    try
    {
        if(!error){
            std::lock_guard<std::mutex> lock(_send_lock);
            if(!_send_que.empty()){
                auto &msgnode  = _send_que.front();
                boost::asio::async_write(_socket,boost::asio::buffer(msgnode->_data,msgnode->_total_len),
                std::bind(&CSession::HandleWrite,this,std::placeholders::_1,shared_self));//当投送节点队列不为空的时候，就把队列的头一个节点内的消息发送
            }
        }
        else{
            std::cout<<"handle write failed,error is "<<error.value()<<std::endl;
            Close();
            _server->ClearSession(_uuid);
            return;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}




void CSession::Send(char *msg,short max_length,short msgid){
    std::lock_guard<std::mutex> lock(_send_lock);
    int send_que_size = _send_que.size();
    if(send_que_size>=MAX_SENDQUE){
        std::cout<<"session "<<_uuid<<" send queue is fulled "<<std::endl;
        return;
    }
    _send_que.push(make_shared<SendNode>(msg,max_length,msgid));
    if(_send_que.size()<0){
        return;
    }
    auto &msgnode = _send_que.front();
    boost::asio::async_write(_socket,boost::asio::buffer(msgnode->_data,msgnode->_total_len),std::bind(&CSession::HandleWrite,this,placeholders::_1,SharedSelf()));
}
void CSession::Send(std::string msg,short msgid){
    std::lock_guard<std::mutex> lock(_send_lock);
    int send_que_size = _send_que.size();
    if(send_que_size>=MAX_SENDQUE){
        std::cout<<"session "<<_uuid<<" send queue is fulled "<<std::endl;
        return;
    }
    _send_que.push(make_shared<SendNode>(msg.c_str(),msg.length(),msgid));
    if(_send_que.size()<0){
        return;
    }
    auto &msgnode = _send_que.front();
    boost::asio::async_write(_socket,boost::asio::buffer(msgnode->_data,msgnode->_total_len),std::bind(&CSession::HandleWrite,this,placeholders::_1,SharedSelf()));
}

std::shared_ptr<CSession>CSession::SharedSelf() {
	return shared_from_this();
}
LogicNode::LogicNode(shared_ptr<CSession>  session, 
	shared_ptr<RecvNode> recvnode):_session(session),_recvnode(recvnode) {
	
}