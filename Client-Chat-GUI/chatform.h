#ifndef CHATFORM_H
#define CHATFORM_H

#include <QDialog>
#include "Message.h"

namespace Ui {
class ChatForm;
}

class ChatForm : public QDialog
{
    Q_OBJECT

public:
    explicit ChatForm(QWidget *parent = nullptr);
    ~ChatForm();

    void setContact(const QString& contactName);
    void setMessages(const std::vector<Message<std::string>>& messages, const QString& login);
    void appendMessage(const QString& from, const QString& text, const QString& login);

signals:
    void sendMessageRequested(const QString& text);
private slots:
    void on_sendBtn_clicked();

private:
    Ui::ChatForm *ui;
};

#endif // CHATFORM_H
