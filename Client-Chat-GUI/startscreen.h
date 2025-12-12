#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QDialog>
#include "ChatClient.h"
#include "loginform.h"
#include "registrationform.h"
namespace Ui {
class startScreen;
}

class startScreen : public QDialog
{
    Q_OBJECT

public:
    explicit startScreen(QWidget *parent = nullptr,
                         std::shared_ptr<ChatClient> chatPtr = nullptr);
    ~startScreen();
    void setLoginForm();
    void setRegistrationForm();

private slots:
    void onLoginRequested(const QString& login, const QString& pass);
    void onRegisterRequest(const QString& login, const QString& pass);

private:
    Ui::startScreen *ui;
    std::shared_ptr<ChatClient> _chatPtr;
};

#endif // STARTSCREEN_H
