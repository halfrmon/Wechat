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
        std::unique_ptr<sql::PreparedStatement>stmt(con->prepareStatement("CALL reg_user(?,?,?,@result)"));//调用存储过程
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

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email) {
    auto con = pool_->getConnection();
    try {
        if (con == nullptr) {
            pool_->returnConnection(std::move(con));
            return false;
        }
        // 准备查询语句
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT email FROM user WHERE name = ?"));
        // 绑定参数
        pstmt->setString(1, name);
        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        // 遍历结果集
        while (res->next()) {
            std::cout << "Check Email: " << res->getString("email") << std::endl;
            if (email != res->getString("email")) {
                pool_->returnConnection(std::move(con));
                return false;
            }
            pool_->returnConnection(std::move(con));
            return true;
        }
    }
    catch (sql::SQLException& e) {
        pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd) {
    auto con = pool_->getConnection();
    try {
        if (con == nullptr) {
            pool_->returnConnection(std::move(con));
            return false;
        }
        // 准备查询语句
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
        // 绑定参数
        pstmt->setString(2, name);
        pstmt->setString(1, newpwd);
        // 执行更新
        int updateCount = pstmt->executeUpdate();
        std::cout << "Updated rows: " << updateCount << std::endl;
        pool_->returnConnection(std::move(con));
        return true;
    }
    catch (sql::SQLException& e) {
        pool_->returnConnection(std::move(con));
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}

bool MysqlDao::CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo) {

    auto con = pool_->getConnection();
    if (con == nullptr) {
        std::cout<<"mysql connect is null"<<std::endl;
        return false;
    }
    Defer defer([this, &con]() {
        pool_->returnConnection(std::move(con));
        });
    try {
        // 准备SQL语句
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("SELECT * FROM user WHERE email = ?"));
        pstmt->setString(1, email); // 将username替换为你要查询的用户名
        // 执行查询
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        std::string origin_pwd = "";
        // 遍历结果集
        while (res->next()) {
            origin_pwd = res->getString("pwd");
            // 输出查询到的密码
            std::cout << "Password: " << origin_pwd << std::endl;
            break;
        }
        if (pwd != origin_pwd) {
            std::cout<<"The pwd is not match,origin_pwd is : "<<origin_pwd<<std::endl;
            return false;
        }
        userInfo.name = res->getString("name");
        userInfo.email = "email";
        userInfo.uid = res->getInt("uid");
        userInfo.pwd = origin_pwd; 
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException: " << e.what();
        std::cerr << " (MySQL error code: " << e.getErrorCode();
        std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
        return false;
    }
}