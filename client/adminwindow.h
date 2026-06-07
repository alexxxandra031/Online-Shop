#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class AdminWindow; }
QT_END_NAMESPACE

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);
    ~AdminWindow();

private:
    Ui::AdminWindow *ui;
};


#endif // ADMINWINDOW_H
