#include "const.h"


class SqlConnection {
public:
	SqlConnection(sql::Connection* con, int64_t lasttime):_con(con), _last_oper_time(lasttime){}
	std::unique_ptr<sql::Connection> _con;
	int64_t _last_oper_time;
};
class MySqlPool
{
public:
    MySqlPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, int poolSize)
        : url_(url), user_(user), pass_(pass), schema_(schema), poolSize_(poolSize), b_stop_(false){
        try {
            for (int i = 0; i < poolSize_; ++i) {
                sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
                std::unique_ptr<sql::Connection> con(driver->connect(url_, user_, pass_));
                con->setSchema(schema_);
                pool_.push(std::move(con));
            }
        }
        catch (sql::SQLException& e) {
            // 处理异常
            std::cout << "mysql pool init failed " <<e.what()<< std::endl;
        }
    }
    std::unique_ptr<sql::Connection> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { 
            if (b_stop_) {
                return true;
            }        
            return !pool_.empty(); });
        if (b_stop_) {
            return nullptr;
        }
        std::unique_ptr<sql::Connection> con(std::move(pool_.front()));
        pool_.pop();
        return con;
    }
    void returnConnection(std::unique_ptr<sql::Connection> con)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if(b_stop_)
        {
            return;
        }
        pool_.push(std::move(con));
        cond_.notify_one();
        return;

    }
    void Close()
    {
        b_stop_=true;
        cond_.notify_all();
    }


private:
    std::string url_;
    std::string user_;
    std::string pass_;
    std::string schema_;
    int poolSize_;
    std::queue<std::unique_ptr<sql::Connection>> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic<bool> b_stop_;
};

struct UserInfo {
	std::string name;
	std::string pwd;
	int uid;
	std::string email;
};
class MysqlDao
{
public:
    MysqlDao();
    ~MysqlDao();
    int RegUser(const std::string& name,const std::string& email,const std::string &pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    
    bool CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo) ;
    
    bool UpdatePwd(const std::string& name, const std::string& pwd);
private:
    std::unique_ptr<MySqlPool> pool_;//mysql连接池中的一个mysql连接
};