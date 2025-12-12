#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QWidget>
#include <startscreen.h>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent,
                       std::shared_ptr<ChatClient> chatPtr)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _chatPtr(chatPtr)
{
    ui->setupUi(this);
    connect(ui->contactsListWidget, &QListWidget::currentRowChanged, this, &MainWindow::onDialogChanged);
    connect(ui->actionConnect_new_client, &QAction::triggered, this, &MainWindow::onNewClientRequested);

    createChats();

    _updateTimer = new QTimer(this);
    _updateTimer->setInterval(5000);
    connect(_updateTimer, &QTimer::timeout, this, &MainWindow::updateMessages);
    _updateTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow *MainWindow::createClient()
{
    auto chat = std::make_shared<ChatClient>();

#ifdef _WIN32
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {

        return nullptr;
    }
#endif

    if (!chat->SetupConnection()) {
#ifdef _WIN32
        WSACleanup();
#endif

        return nullptr;
    }
    startScreen s(nullptr, chat);
    int result = s.exec();

    if (result == QDialog::Accepted) {
        auto w = new MainWindow(nullptr, chat);
        return w;
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return nullptr;
}

void MainWindow::updateCurrentChat()
{
    int row = ui->contactsListWidget->currentRow();
    if (row < 0 || row >= ui->chatsStackedWidget->count())
        return;

    auto* chatForm = qobject_cast<ChatForm*>(ui->chatsStackedWidget->widget(row));
    if (!chatForm)
        return;

    auto* item = ui->contactsListWidget->item(row);
    if (!item)
        return;

    const std::string contact = item->text().toStdString();
    const auto messages = _chatPtr->getMessages();
    const QString self = QString::fromStdString(_chatPtr->getCurrentUser());

    std::vector<Message<std::string>> dialogMessages;
    dialogMessages.reserve(messages.size());

    for (const auto& msg : messages)
    {
        const std::string& from = msg.getFrom();
        const std::string& to   = msg.getTo();

        if (contact == "common")
        {
            if (to == "common")
                dialogMessages.push_back(msg);
        }
        else
        {
            if ((from == _chatPtr->getCurrentUser() && to == contact) ||
                (to   == _chatPtr->getCurrentUser() && from == contact))
            {
                dialogMessages.push_back(msg);
            }
        }
    }
    chatForm->setMessages(dialogMessages, self);
}

void MainWindow::createChats()
{
    // --- 1. Запоминаем текущий выбранный контакт
    QString prevContact;
    if (ui->contactsListWidget->currentItem())
        prevContact = ui->contactsListWidget->currentItem()->text();

    // --- 2. Полная пересборка (как у тебя)
    ui->contactsListWidget->clear();

    while (ui->chatsStackedWidget->count() > 0)
    {
        QWidget* w = ui->chatsStackedWidget->widget(0);
        ui->chatsStackedWidget->removeWidget(w);
        w->deleteLater();
    }

    const auto contacts = _chatPtr->getContacts();
    const auto messages = _chatPtr->getMessages();
    const QString self  = QString::fromStdString(_chatPtr->getCurrentUser());

    auto addChatForContact = [&](const std::string& name)
    {
        if (name.empty())
            return;

        QString contactName = QString::fromStdString(name);
        ui->contactsListWidget->addItem(contactName);

        std::vector<Message<std::string>> dialogMessages;

        for (const auto& msg : messages)
        {
            const std::string& from = msg.getFrom();
            const std::string& to   = msg.getTo();

            if (name == "common")
            {
                if (to == "common")
                    dialogMessages.push_back(msg);
            }
            else
            {
                if ((from == _chatPtr->getCurrentUser() && to == name) ||
                    (to   == _chatPtr->getCurrentUser() && from == name))
                {
                    dialogMessages.push_back(msg);
                }
            }
        }

        auto* chatForm = new ChatForm(this);
        chatForm->setContact(contactName);
        chatForm->setMessages(dialogMessages, self);
        connect(chatForm, &ChatForm::sendMessageRequested,
                this, &MainWindow::onChatMessageSendRequested);

        ui->chatsStackedWidget->addWidget(chatForm);
    };

    // common — первым
    for (const auto& name : contacts)
    {
        if (name == "common")
        {
            addChatForContact(name);
            break;
        }
    }

    for (const auto& name : contacts)
    {
        if (!name.empty() && name != "common")
            addChatForContact(name);
    }

    // --- 3. ВОССТАНАВЛИВАЕМ ВЫБОР, А НЕ ПРЫГАЕМ НА 0
    int restoreRow = -1;
    if (!prevContact.isEmpty())
    {
        for (int i = 0; i < ui->contactsListWidget->count(); ++i)
        {
            if (ui->contactsListWidget->item(i)->text() == prevContact)
            {
                restoreRow = i;
                break;
            }
        }
    }

    if (restoreRow >= 0)
        ui->contactsListWidget->setCurrentRow(restoreRow);
    else if (ui->contactsListWidget->count() > 0)
        ui->contactsListWidget->setCurrentRow(0);

    ui->currentLoginLabel->setText(
        tr("Current user: %1")
            .arg(QString::fromStdString(_chatPtr->getCurrentUser()))
        );
}

void MainWindow::onDialogChanged(int row)
{
    if (row >= 0 && row < ui->chatsStackedWidget->count())
        ui->chatsStackedWidget->setCurrentIndex(row);
}

void MainWindow::onNewClientRequested()
{
    if (auto w = MainWindow::createClient())
    {
        w->show();
    }
}

void MainWindow::on_actionConnect_new_client_triggered()
{
    onNewClientRequested();
}

void MainWindow::on_actionClose_client_triggered()
{
    close();
}

void MainWindow::onChatMessageSendRequested(const QString &text)
{
    auto* chatForm = qobject_cast<ChatForm*>(sender());
    if (!chatForm)
        return;

    int index = ui->chatsStackedWidget->indexOf(chatForm);
    if (index < 0 || index >= ui->contactsListWidget->count())
        return;

    QString contactName = ui->contactsListWidget->item(index)->text();
    std::string to      = contactName.toStdString();
    std::string msgText = text.toStdString();

    if (!_chatPtr->sendMessage(to, msgText))
    {
        QMessageBox::warning(this, tr("Error"),
                             tr("Message not sends"));
        return;
    }

    const QString self = QString::fromStdString(_chatPtr->getCurrentUser());
    chatForm->appendMessage(self, text, self);
}

void MainWindow::updateMessages()
{
    if (!_chatPtr)
        return;

    if (!_chatPtr->requestAllMessages())
    {
        if (_updateTimer)
            _updateTimer->stop();

        QMessageBox::warning(this,
                             tr("Disconnected"),
                             tr("Connection to server lost. The client will be closed."));
        close();
        return;
    }
    updateCurrentChat();
}


