#include <QApplication>
#include "authwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    AuthWindow authWin;
    authWin.show();
    
    return a.exec();
}
