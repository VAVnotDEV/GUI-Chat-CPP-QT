#include "startscreen.h"
#include "ui_startscreen.h"
#include <QLabel>
#include <QMessageBox>

startScreen::startScreen(QWidget *parent ,std::shared_ptr<ChatClient> chatPtr)
    : QDialog(parent)
    , ui(new Ui::startScreen)
    , _chatPtr(chatPtr)
{
    ui->setupUi(this);

    connect(ui->loginPage, &loginForm::registerRequested, this, &startScreen::setRegistrationForm);
    connect(ui->loginPage, &loginForm::accepted, this, &startScreen::onLoginRequested);
    connect(ui->loginPage, &loginForm::rejected, this, &startScreen::reject);

    connect(ui->registrationPage, &registrationForm::loginRequested, this, &startScreen::setLoginForm);
    connect(ui->registrationPage, &registrationForm::accepted, this, &startScreen::onRegisterRequest);
    connect(ui->registrationPage, &registrationForm::rejected, this, &startScreen::reject);
}

startScreen::~startScreen()
{
    delete ui;
}

void startScreen::setLoginForm()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void startScreen::setRegistrationForm()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void startScreen::onLoginRequested(const QString &login, const QString &pass)
{
    bool bAuth = _chatPtr->authUser(login.toStdString(), pass.toStdString());

    if(bAuth)
    {
        accept();
        _chatPtr->getListUser();
      //  _chatPtr->recvAllMessageFrom();
    }
    else
        QMessageBox::critical(this, tr("Error"), tr("Incorrent login and password"));
}

void startScreen::onRegisterRequest(const QString &login, const QString &pass)
{
    if(_chatPtr->userRegister(login.toStdString(), pass.toStdString()))
        onLoginRequested(login, pass);

    else
        QMessageBox::critical(this, tr("Error"), tr("Login already exist!"));

}
