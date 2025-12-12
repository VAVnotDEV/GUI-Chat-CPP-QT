#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include "ChatClient.h"
#include <memory>
#include "chatform.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr,
               std::shared_ptr<ChatClient> chatPtr = nullptr);
    ~MainWindow();

    static MainWindow* createClient();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<ChatClient> _chatPtr;
    QTimer* _updateTimer;
    void updateCurrentChat();
    void createChats();
    bool _firstBuild = true;

private slots:
    void onDialogChanged(int row);
    void onNewClientRequested();
    void on_actionConnect_new_client_triggered();
    void on_actionClose_client_triggered();
    void onChatMessageSendRequested(const QString& text);
    void updateMessages();
};
#endif // MAINWINDOW_H
