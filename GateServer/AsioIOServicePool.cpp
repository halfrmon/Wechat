#include "AsioIOServicePool.h"


AsioIOServicePool::AsioIOServicePool(std::size_t size):_ioServices(size),_works(size),_nextIOService(0)
{
    for(size_t i=0;i<size;++i)
    {
        _works[i] = std::unique_ptr<Work>(new Work(_ioServices[i]));//所有的iocontext都由一个workptr管理
    }

    for(size_t i=0;i<_ioServices.size();++i)
    {
        _threads.emplace_back([this,i](){
            _ioServices[i].run();//遍历多个ioservices，创建多个线程，每个线程中启动一个iocontext
        });
    }
}

boost::asio::io_context& AsioIOServicePool:: GetIOService()
{
    auto &service = _ioServices[_nextIOService++];
    if(_nextIOService==_ioServices.size())
    {
        _nextIOService = 0;
    }
    return service;
}

AsioIOServicePool::~AsioIOServicePool()
{
    Stop();
    std::cout<<"AsioServicePool destruct"<<std::endl;
}

void AsioIOServicePool::Stop()
{
    for (auto&work :_works)
    {
        work->get_io_context().stop();
        work.reset();
    }

    for (auto &t:_threads)
    {
        t.join();
    }
}