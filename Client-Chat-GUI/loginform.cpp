#include "loginform.h"
#include "ui_loginform.h"

loginForm::loginForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::loginForm)
{
    ui->setupUi(this);
}

loginForm::~loginForm()
{
    delete ui;
}

void loginForm::on_dialogbtnBox_accepted()
{
    const QString login = ui->loginLineEdit->text();
    const QString password = ui->passwordLineEdit->text();

    if(login.isEmpty() || password.isEmpty())
    {
        ui->signLabel->setText(tr("Enter login and password!"));
        return;
    }

    emit accepted(login, password);
}


void loginForm::on_dialogbtnBox_rejected()
{
    emit rejected();
}


void loginForm::on_RegistrationBtn_clicked()
{
    emit registerRequested();
}

