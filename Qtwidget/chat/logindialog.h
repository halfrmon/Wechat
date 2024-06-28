#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    void showTip(QString str,bool b_ok);

private:
    Ui::LoginDialog *ui;
    void initHead();
    bool checkUserVaild();
    bool checkPwdVaild();
    void initHttpHandlers();

public slots:
    void slot_forget_pwd();
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
signals:
    void switchRegister();
    void switchReset();
private slots:
    void on_login_btn_clicked();

private:
    QMap<ReqId,std::function<void(const QJsonObject&)>> _handlers;
};

#endif // LOGINDIALOG_H
