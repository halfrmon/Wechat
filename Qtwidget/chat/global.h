#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include "QStyle"
#include <QRegularExpression>
#include <memory>
#include <iostream>
#include <mutex>
#include <QDir>
#include <QSettings>



extern std::function<void(QWidget*)> repolish;
extern QString gate_url_prefix;

enum ReqId{
    ID_GET_VARIFY_CODE = 1001,//获取验证码
    ID_REG_USER = 1002,//注册用户

};

enum Modules{
  REGISTERMOD = 0,
};

enum ErrorCodes{
  SUCESS = 0,
  ERR_JSON = 1,//JSON解析失败
  ERR_NETWORK = 2,//网络错误
};


enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};
enum ClickLbState{
    Normal = 0,
    Selected = 1
};
#endif // GLOBAL_H
