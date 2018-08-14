#include "headers/connectdb.h"
#include "headers/workerplace.h"
#include <QtWidgets>
#include <QtWidgets/QApplication>
#include <QTextCodec>
#include <windows.h>
#include <signal.h>

void handler_sigsegv(int signum)
{
    MessageBoxA(NULL,"SIGSEGV Error!","POSIX Signal",MB_ICONSTOP);
    // открепить обработчик и явно завершить приложение
    signal(signum, SIG_DFL);
    exit(3);
}

void handler_sigfpe(int signum)
{
    MessageBoxA(NULL,"SIGFPE Error!","POSIX Signal",MB_ICONSTOP);
    // открепить обработчик и явно завершить приложение
    signal(signum, SIG_DFL);
    exit(3);
}

int main(int argc, char *argv[])
{
    // установим наши обработчики на два сигнала
    signal(SIGSEGV, handler_sigsegv);
    signal(SIGFPE, handler_sigfpe);

    Q_INIT_RESOURCE(icons);
    QApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codec);

    connectDB cdb;
    cdb.setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    if(cdb.exec()){
        workerPlace w;
        w.showMaximized();
        return a.exec();
    }else
        return 0;
}
