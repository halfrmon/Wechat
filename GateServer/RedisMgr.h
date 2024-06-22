//对redis进行封装
#include "const.h"
#include "Singleton.h"


//封装redis连接池

class RedisConPool
{
public:
    RedisConPool(size_t poolSize,const char*host,int port,const char* pwd):
    poolSize_(poolSize),host_(host),port_(port),b_stop_(false){
        for(size_t i=0;i<poolSize_;++i)
        {
            auto* context = redisConnect(host,port);
            if (context == nullptr||context->err!=0)
            {
                if(context!=nullptr)
                {
                    redisFree(context);
                }
                continue;
            }


            auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
            if (reply->type == REDIS_REPLY_ERROR) {
                std::cout << "认证失败" << std::endl;
                //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
                freeReplyObject(reply);
                continue;
            }
            //执行成功 释放redisCommand执行后返回的redisReply所占用的内存
            freeReplyObject(reply);
            std::cout << "认证成功" << std::endl;
            connections_.push(context);
        }
    };
    ~RedisConPool()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        while(!connections_.empty())
        {
            connections_.pop();
        }
    }
    redisContext* getConnection()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock,[this]
        {
            if(b_stop_)
            {
                return true;
            }
            return !connections_.empty();
        });

        if(b_stop_)
        {
            return nullptr;
        }

        auto* context = connections_.front();
        connections_.pop();
        return context;
    }

    void returnConnection(redisContext*context)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if(b_stop_)
        {
            return;
        }
        connections_.push(context);
        cond_.notify_one();

    }
    void Close()
    {
        b_stop_ = true;
        cond_.notify_all();
    }




private:
    size_t poolSize_;
    const char* host_;
    int port_;

    std::atomic<bool> b_stop_;
    std::queue<redisContext*> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};


class RedisMgr: public Singleton<RedisMgr>, 
    public std::enable_shared_from_this<RedisMgr>
{
    friend class Singleton<RedisMgr>;
public:
    ~RedisMgr();
    bool Connect(const std::string& host, int port);//连接
    bool Get(const std::string &key, std::string& value);//根据对应的key获取alue
    bool Set(const std::string &key, const std::string &value);//设置key和alue
    bool Auth(const std::string &password);//密码认证
    bool LPush(const std::string &key, const std::string &value);
    bool LPop(const std::string &key, std::string& value);
    bool RPush(const std::string& key, const std::string& value);
    bool RPop(const std::string& key, std::string& value);
    bool HSet(const std::string &key, const std::string  &hkey, const std::string &value);
    bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
    std::string HGet(const std::string &key, const std::string &hkey);
    bool Del(const std::string &key);
    bool ExistsKey(const std::string &key);//判断键值是否存在
    void Close()
    {
        _con_pool->Close();
    };
private:
    RedisMgr();
	std::unique_ptr<RedisConPool>  _con_pool;
};