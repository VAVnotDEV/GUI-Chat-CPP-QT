QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ChatClient.cpp \
    DataExchangeProtocol.cpp \
    User.cpp \
    chatform.cpp \
    loginform.cpp \
    main.cpp \
    mainwindow.cpp \
    registrationform.cpp \
    startscreen.cpp

HEADERS += \
    ChatClient.h \
    DataExchangeProtocol.h \
    Message.h \
    User.h \
    chatform.h \
    loginform.h \
    mainwindow.h \
    registrationform.h \
    startscreen.h

FORMS += \
    chatform.ui \
    loginform.ui \
    mainwindow.ui \
    registrationform.ui \
    startscreen.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
