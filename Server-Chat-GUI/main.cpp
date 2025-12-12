#include "mainwindow.h"
#include <QApplication>

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
    MainWindow w;
    w.show();
    int result = a.exec();

#ifdef _WIN32
    WSACleanup();
#endif

    return result;
}
