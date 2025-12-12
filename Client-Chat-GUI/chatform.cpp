#include "chatform.h"
#include "ui_chatform.h"


ChatForm::ChatForm(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatForm)
{
    ui->setupUi(this);

    connect(ui->sendBtn, &QPushButton::clicked,
            this, &ChatForm::on_sendBtn_clicked);

    connect(ui->messageLineEdit, &QLineEdit::returnPressed,
            this, &ChatForm::on_sendBtn_clicked);
}

ChatForm::~ChatForm()
{
    delete ui;
}

void ChatForm::setContact(const QString &contactName)
{
    ui->currentContactLabel->setText(contactName);
}

void ChatForm::setMessages(const std::vector<Message<std::string> > &messages, const QString &login)
{
    ui->MessagesEdit->clear();

    for (const auto& m : messages)
    {
        const QString from  = QString::fromStdString(m.getFrom());
        const QString text  = QString::fromStdString(m.getMessage());

        QString line;
        if (from == login)
            line = QStringLiteral("I: %1").arg(text);
        else
            line = QStringLiteral("%1: %2").arg(from, text);

        ui->MessagesEdit->append(line);
    }


    QTextCursor c = ui->MessagesEdit->textCursor();
    c.movePosition(QTextCursor::End);
    ui->MessagesEdit->setTextCursor(c);
}

void ChatForm::appendMessage(const QString &from, const QString &text, const QString &login)
{
    QString line;
    if (from == login)
        line = QStringLiteral("Я: %1").arg(text);
    else
        line = QStringLiteral("%1: %2").arg(from, text);

    ui->MessagesEdit->append(line);

    QTextCursor c = ui->MessagesEdit->textCursor();
    c.movePosition(QTextCursor::End);
    ui->MessagesEdit->setTextCursor(c);

}

void ChatForm::on_sendBtn_clicked()
{
    const QString text = ui->messageLineEdit->text();
    if (text.isEmpty())
        return;

    emit sendMessageRequested(text);
    ui->messageLineEdit->clear();
}

