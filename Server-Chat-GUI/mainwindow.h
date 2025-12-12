#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ChatServer.h"
#include <thread>
#include <mutex>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateUsersList();
    void updateMessagesList();
    QString selectedLogin() const;



private slots:
    void on_banUserPushButton_clicked();
    void on_enableSeverAction_triggered();
    void on_stopServerAction_triggered();



    void on_kickUserPushButton_clicked();

    void on_getListUsersPushButton_clicked();

private:
    Ui::MainWindow *ui;
    Chat _chat;
    std::string currentLogin;
    int _usersCount;
    int _messagesPrivateCount;
    int _messagesCommonCount;
    std::thread _serverThread;
    std::map<QString, SocketType> _clientSockets;
    std::mutex _clientsMutex;
    QTimer* _updateTimer;
    void startServer();
    void stopServer();
    void handleClient(SocketType client_fd);
    void rememberClient(SocketType fd, const QString& login);
    void forgetClient(SocketType fd);
};
#endif // MAINWINDOW_H
