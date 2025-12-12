#include "mainwindow.h"
#include "ChatClient.h"
#include <QApplication>
#include <memory>

int main(int argc, char *argv[])
{

#ifdef _WIN32
    WSADATA wsaData;
    int wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        return -1;
    }
#endif

    QApplication a(argc, argv);
  //  auto chat = std::make_shared<ChatClient>();
    auto w = MainWindow::createClient();

    if(w)
    w->show();
    else
        return 0;
    int result = a.exec();

#ifdef _WIN32
    WSACleanup();
#endif

    return result;
}
