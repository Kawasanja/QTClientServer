#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w("localhost", 1212);
    w.show();

    return a.exec();
}
