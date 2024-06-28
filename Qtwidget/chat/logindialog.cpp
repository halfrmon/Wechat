#include "logindialog.h"
#include "ui_logindialog.h"
#include "QDebug"
#include <QPainter>
#include <QPainterPath>
#include <QJsonObject>
#include "httpmgr.h"
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    connect(ui->reg_btn,&QPushButton::clicked,this,&LoginDialog::switchRegister);
    ui->forget_label->SetState("normal","hover","","selected","selected_hover","");
    ui->forget_label->setCursor(Qt::PointingHandCursor);
    connect(ui->forget_label,&ClickedLabel::clicked,this,&LoginDialog::slot_forget_pwd);
    initHead();
    //连接登陆信号
    connect(HttpMgr::GetInstance().get(),&HttpMgr::sig_login_mode_finish,this,&LoginDialog::slot_login_mod_finish);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::showTip(QString str, bool b_ok)
{
    if(b_ok)
    {
        ui->err_tip->setProperty("state","normal");
    }else
    {
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void LoginDialog::initHead()
{
    // 加载图片
    QPixmap originalPixmap(":/res/head_2.jpg");
      // 设置图片自动缩放
    qDebug()<< originalPixmap.size() << ui->ead_label->size();
    originalPixmap = originalPixmap.scaled(ui->ead_label->size(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建一个和原始图片相同大小的QPixmap，用于绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent); // 用透明色填充

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing); // 设置抗锯齿，使圆角更平滑
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // 使用QPainterPath设置圆角
    QPainterPath path;
    path.addRoundedRect(0, 0, originalPixmap.width(), originalPixmap.height(), 10, 10); // 最后两个参数分别是x和y方向的圆角半径
    painter.setClipPath(path);

    // 将原始图片绘制到roundedPixmap上
    painter.drawPixmap(0, 0, originalPixmap);
    // 设置绘制好的圆角图片到QLabel上
    ui->ead_label->setPixmap(roundedPixmap);

}

bool LoginDialog::checkUserVaild()
{
    auto user = ui->email_edit->text();
    if(user.isEmpty()){
        qDebug()<<"User empty ";
        return false;
    }
    return true;
}

bool LoginDialog::checkPwdVaild()
{
    auto pwd = ui->pass_edit->text();
    if(pwd.length()<6||pwd.length()>15){
        qDebug()<<"Pass length invaild";
        return false;
    }
    return true;
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_LOGIN_USER,[this](QJsonObject jsonobj){
        int error = jsonobj["error"].toInt();
        if(error!=ErrorCodes::SUCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto user = jsonobj["user"].toString();
        showTip(tr("登陆成功"),true);
        return;
    });
}

void LoginDialog::slot_forget_pwd()
{
    qDebug()<<"slot forget pwd";
    emit switchReset();
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCESS){
        showTip(tr("网络请求错误"),false);
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("json解析错误"),false);
        return;
    }
    if(!jsonDoc.isObject()){
        showTip(tr("json解析错误"),false);
        return;
    }

    _handlers[id](jsonDoc.object());
    return;
}

void LoginDialog::on_login_btn_clicked()
{
    qDebug()<<"login btn clicked";
    if(checkUserVaild() == false){
        return;
    }
    if(checkPwdVaild() == false){
        return ;
    }
    auto user = ui->email_edit->text();
    auto pwd = ui->pass_edit->text();
    //发送http请求登陆
    QJsonObject json_obj;
    json_obj["user"] = user;
    json_obj["passwd"] = pwd;
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),
                                        json_obj, ReqId::ID_LOGIN_USER,Modules::LOGINMOD);

}

