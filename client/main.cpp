#include <QApplication>
#include "authwindow.h"
#include "clientmanager.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    ClientManager::getInstance()->connectToServer("127.0.0.1", 33333);
    AuthWindow authWin;
    authWin.show();
    
    return a.exec();
}
