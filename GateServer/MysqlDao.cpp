#include "MysqlDao.h"
#include "ConfigMgr.h"


MysqlDao::MysqlDao()
{
    auto & cfg = ConfigMgr::Inst();
    const auto &host = cfg["Mysql"]["Host"];
    const auto &port = cfg["Mysql"]["Port"];
    const auto &pwd = cfg["Mysql"]["Passwd"];
    const auto &schema = cfg["Mysql"]["Schema"];
    const auto &user = cfg["Mysql"]["User"];
    pool_.reset(new MySqlPool(host+":"+port,user,pwd,schema,5)); 
}


MysqlDao::~MysqlDao()
{
    pool_->Close();
}

int MysqlDao::RegUser(const std::string &name,const std::string &email,const std::string &pwd)
{
    auto con = pool_->getConnection();
    try
    {
        if(con == nullptr)
        {
            pool_->returnConnection(std::move(con));
            return false;
        }
        std::unique_ptr<sql::PreparedStatement>stmt(con->prepareStatement("CALL_reg_user(?,?,?,@result)"));//调用存储过程
        //设置输入参数
        stmt->setString(1,name);
        stmt->setString(2,email);
        stmt->setString(3,pwd);

        stmt->execute();
        std::unique_ptr<sql::Statement>stmtResult(con->createStatement());
        std::unique_ptr<sql::ResultSet>res(stmtResult->executeQuery("SELECT @result AS result"));
        if(res->next())
        {
            int result = res->getInt("result");
            std::cout<<"Result: "<<result<<std::endl;
            pool_->returnConnection(std::move(con));
            return result;
        }
        pool_->returnConnection(std::move(con));
        return -1;


    }
    catch(const std::exception& e)
    {
        pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        return -1;
    }
    
}

