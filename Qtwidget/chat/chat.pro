QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
RC_ICONS = icon.ico

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clickedlabel.cpp \
    global.cpp \
    httpmgr.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    registerdialog.cpp \
    resetdialog.cpp \
    tcpmgr.cpp \
    timerbtn.cpp

HEADERS += \
    clickedlabel.h \
    global.h \
    httpmgr.h \
    logindialog.h \
    mainwindow.h \
    registerdialog.h \
    resetdialog.h \
    singleton.h \
    tcpmgr.h \
    timerbtn.h

FORMS += \
    logindialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    resetdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rc.qrc

DISTFILES += \
    ../../../llfcchat-master/client/llfcchat/res/add_friend.png \
    ../../../llfcchat-master/client/llfcchat/res/unvisible.png \
    ../../../llfcchat-master/client/llfcchat/res/unvisible_hover.png \
    ../../../llfcchat-master/client/llfcchat/res/visible.png \
    ../../../llfcchat-master/client/llfcchat/res/visible_hover.png \
    config.ini \
    res/add_friend_hover.png \
    res/unvisible.png \
    res/unvisible_hover.png \
    res/visible.png \
    res/visible_hover.png
