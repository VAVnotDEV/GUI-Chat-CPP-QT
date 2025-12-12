#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _usersCount(0)
    , _messagesPrivateCount(0)
    , _messagesCommonCount(0)

{
    ui->setupUi(this);

    //User List Table
    ui->userListTableWidget->setColumnCount(3);
    ui->userListTableWidget->setHorizontalHeaderLabels({tr("UserName"), tr("Banned"), tr("Online")});
    ui->userListTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->userListTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->userListTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    //Private messages list table
    ui->privateMessageListTableWidget->setColumnCount(3);
    ui->privateMessageListTableWidget->setHorizontalHeaderLabels({tr("Sender"), tr("Text"), tr("Recipient")});
    ui->privateMessageListTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->privateMessageListTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->privateMessageListTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    //Common message list table
    ui->commonMessageListTableWidget->setColumnCount(2);
    ui->commonMessageListTableWidget->setHorizontalHeaderLabels({tr("Sender"), tr("Text")});
    ui->commonMessageListTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->commonMessageListTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->commonMessageListTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->serverStateLabel->setText(tr("Server: Stop"));

    updateUsersList();
    updateMessagesList();

    _updateTimer = new QTimer(this);
    _updateTimer->setInterval(5000);
    connect(_updateTimer, &QTimer::timeout, this, &MainWindow::updateUsersList);
    connect(_updateTimer, &QTimer::timeout, this, &MainWindow::updateMessagesList);
    _updateTimer->start();

}

MainWindow::~MainWindow()
{
    stopServer();
    delete ui;
}

void MainWindow::updateUsersList()
{
    auto users = _chat.getUsers();
    _usersCount = users.size();
    ui->userListTableWidget->setRowCount(_usersCount);

    for(int i = 0; i < _usersCount; i++)
    {
        ui->userListTableWidget->setItem(
            i, 0,
            new QTableWidgetItem(QString::fromStdString(users[i].getName()))
            );

        QString banned = users[i].isBanned() ? "Yes" : "No";
        ui->userListTableWidget->setItem(
            i, 1,
            new QTableWidgetItem(banned)
            );

        QString online = users[i].isOnline() ? "Yes" : "No";
        ui->userListTableWidget->setItem(
            i, 2,
            new QTableWidgetItem(online)
            );
    }

    ui->userCountLabel->setText(tr("Users count: %1").arg(_usersCount));
}

void MainWindow::updateMessagesList()
{
    auto messages = _chat.getAllMessages();

    _messagesPrivateCount = 0;
    _messagesCommonCount  = 0;

    // 1. Подсчитываем строки
    for (const auto& m : messages)
    {
        QString to = QString::fromStdString(m.getTo());
        if (to == QStringLiteral("common"))
            ++_messagesCommonCount;
        else
            ++_messagesPrivateCount;
    }

    // 2. Настраиваем таблицы
    ui->privateMessageListTableWidget->setRowCount(_messagesPrivateCount);
    ui->commonMessageListTableWidget->setRowCount(_messagesCommonCount);

    ui->privateMessageListTableWidget->clearContents();
    ui->commonMessageListTableWidget->clearContents();

    // 3. Заполняем таблицы
    int i = 0;
    int j = 0;

    for (const auto& m : messages)
    {
        QString from = QString::fromStdString(m.getFrom());
        QString to   = QString::fromStdString(m.getTo());
        QString text = QString::fromStdString(m.getMessage());

        if (to == QStringLiteral("common"))
        {
            ui->commonMessageListTableWidget->setItem(i, 0, new QTableWidgetItem(from));
            ui->commonMessageListTableWidget->setItem(i, 1, new QTableWidgetItem(text));
            ++i;
        }
        else
        {
            ui->privateMessageListTableWidget->setItem(j, 0, new QTableWidgetItem(from));
            ui->privateMessageListTableWidget->setItem(j, 1, new QTableWidgetItem(text));
            ui->privateMessageListTableWidget->setItem(j, 2, new QTableWidgetItem(to));
            ++j;
        }
    }

    // 4. Обновляем счётчики
    ui->privateMessagesCountLabel->setText(
        tr("Private messages count: %1").arg(_messagesPrivateCount));

    ui->commonMessagesCountLabel->setText(
        tr("Common messages count: %1").arg(_messagesCommonCount));
}

QString MainWindow::selectedLogin() const
{
    int row = ui->userListTableWidget->currentRow();
     if (row < 0) return "";

    return ui->userListTableWidget->item(row, 0)->text();
}

void MainWindow::on_banUserPushButton_clicked()
{
    QString login = selectedLogin();
    if (login.isEmpty()) return;

    std::string l = login.toStdString();

    if (_chat.banCheck(l))
    {
        _chat.unbanUser(l);
    }
    else
    {
        _chat.banUser(l);

        std::lock_guard<std::mutex> lock(_clientsMutex);
        auto it = _clientSockets.find(login);
        if (it != _clientSockets.end())
        {
#ifdef _WIN32
            closesocket(it->second);
#else
            close(it->second);
#endif
            _clientSockets.erase(it);
            _chat.setUserOnline(l, false);
        }
    }

    updateUsersList();
}

void MainWindow::on_enableSeverAction_triggered()
{
    startServer();
}

void MainWindow::on_stopServerAction_triggered()
{
    stopServer();
}

void MainWindow::startServer()
{
    if (_chat.isRunning())
        return;

    _serverThread = std::thread([this]()
    {
        if (!_chat.run())
            return;

        QMetaObject::invokeMethod(this, [this]()
        {
            ui->serverStateLabel->setText(tr("Server: Run"));
        }, Qt::QueuedConnection);

        while (_chat.isRunning())
        {
            SocketType client_fd = _chat.acceptClient();

            if (!_chat.isRunning())
                break;

#ifdef _WIN32
            if (client_fd == INVALID_SOCKET)
#else
            if (client_fd < 0)
#endif
            {
                continue;
            }
            std::thread([this, client_fd]()
                        {
                            handleClient(client_fd);
                        }).detach();
        }
    });
}
void MainWindow::stopServer()
{
    if (!_chat.isRunning())
        return;
    _chat.stop();

    if (_serverThread.joinable())
        _serverThread.join();

    ui->serverStateLabel->setText(tr("Server: Stop"));
}

void MainWindow::handleClient(SocketType client_fd)
{
    bool clientAlive = true;
    std::string currentLogin;

    while (_chat.isRunning() && clientAlive && _chat.isRunning())
    {
        DataExchangeProtocol dep;

        if (!_chat.recvData(client_fd, dep))
        {
            if (!currentLogin.empty())
                _chat.setUserOnline(currentLogin, false);

            clientAlive = false;
            break;
        }

        // Регистрация нового пользователя
        if (dep.cmd == "ADD_USER")
        {

            if (_chat.validateAddUser(dep.login))
                _chat.addUser(dep);
            else
                dep.log = "0";

            _chat.sendData(client_fd, dep);
        }
        else if (dep.cmd == "AUTH_USER")
        {
                if (_chat.loginUser(client_fd, dep))
                {
                    currentLogin = dep.login;
                    rememberClient(client_fd, QString::fromStdString(dep.login));
                }

                _chat.sendData(client_fd, dep);
        }
        else if (dep.cmd == "GET_USERS")
        {
            _chat.sendUsersToClient(client_fd, dep);
        }
        else if (dep.cmd == "SEND_MESSAGE")
        {
            _chat.recvMessageToServer(dep);
        }
        else if (dep.cmd == "RECV_MESSAGE")
        {
            _chat.sendMessageToClient(client_fd, dep);
        }
        else if (dep.cmd == "GET_ALL_MESSAGES")
        {
            _chat.sendAllMessages(client_fd, dep);
        }
        else if (dep.cmd == "EXIT")
        {
            if (!currentLogin.empty())
            {
                _chat.setUserOnline(currentLogin, false);
            }

            dep.DataClear();
            clientAlive = false;
        }
        else
        {
            continue;
        }
    }

    if (!currentLogin.empty())
    {
        _chat.setUserOnline(currentLogin, false);
    }

    forgetClient(client_fd);

#ifdef _WIN32
    closesocket(client_fd);
#else
    close(client_fd);
#endif
}

void MainWindow::rememberClient(SocketType fd, const QString &login)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    _clientSockets[login] = fd;
}

void MainWindow::forgetClient(SocketType fd)
{
    std::lock_guard<std::mutex> lock(_clientsMutex);
    for (auto it = _clientSockets.begin(); it != _clientSockets.end();)
    {
        if (it->second == fd)
            it = _clientSockets.erase(it);
        else
            ++it;
    }
}

void MainWindow::on_kickUserPushButton_clicked()
{
    QString login = selectedLogin();
    if (login.isEmpty())
        return;

    std::string loginStd = login.toStdString();

    SocketType fd = -1;
    {
        std::lock_guard<std::mutex> lock(_clientsMutex);
        auto it = _clientSockets.find(login);
        if (it == _clientSockets.end())
        {
            updateUsersList();
            return;
        }

        fd = it->second;
        _clientSockets.erase(it);
    }

#ifdef _WIN32
    closesocket(fd);
#else
    close(fd);
#endif

    _chat.setUserOnline(loginStd, false);

    updateUsersList();
}


void MainWindow::on_getListUsersPushButton_clicked()
{
    updateUsersList();
}

