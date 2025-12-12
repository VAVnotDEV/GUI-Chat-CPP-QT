#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
#include <ChatClient.h>

namespace Ui {
class loginForm;
}

class loginForm : public QDialog
{
    Q_OBJECT

public:
    explicit loginForm(QWidget *parent = nullptr);
    ~loginForm();

signals:
    void registerRequested();
    void accepted(const QString& login, const QString& password);
    void rejected();

private slots:
    void on_dialogbtnBox_accepted();
    void on_dialogbtnBox_rejected();
    void on_RegistrationBtn_clicked();

private:
    Ui::loginForm *ui;\
};

#endif // LOGINFORM_H
