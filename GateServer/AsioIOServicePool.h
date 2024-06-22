#include <boost/asio.hpp>
#include <vector>
#include "Singleton.h"



/*提高gateserver的并发，采用线程池的方法*/

class AsioIOServicePool:public Singleton<AsioIOServicePool>
{
    friend class Singleton<AsioIOServicePool>;

public:
    using IOService = boost::asio::io_context;
    using Work = boost::asio::io_context::work;
    using WorkPtr = std::unique_ptr<Work>;//声明命名空间

    ~AsioIOServicePool();
    AsioIOServicePool(const AsioIOServicePool&) = delete;
    AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;//禁用拷贝和赋值构造

    boost::asio::io_context &GetIOService();
    void Stop();
private:
    AsioIOServicePool(std::size_t size = 2);
    std::vector<IOService>  _ioServices;
    std::vector<WorkPtr> _works;
    std::vector<std::thread> _threads;
    std::size_t             _nextIOService;

};