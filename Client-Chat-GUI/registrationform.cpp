#include "registrationform.h"
#include "ui_registrationform.h"
#include <QMessageBox>

registrationForm::registrationForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::registrationForm)
{
    ui->setupUi(this);
}

registrationForm::~registrationForm()
{
    delete ui;
}

void registrationForm::on_dialogButton_accepted()
{
    if(ui->passwordLineEdit->text() != ui->confirmPasswordLineEdit->text())
    {
        QMessageBox::critical(this, tr("Error"), tr("Password not match"));
        return;
    }
    accepted(ui->loginLineEdit->text(), ui->passwordLineEdit->text());
}


void registrationForm::on_dialogButton_rejected()
{
    emit rejected();
}


void registrationForm::on_LoginBtn_clicked()
{
    emit loginRequested();
}

