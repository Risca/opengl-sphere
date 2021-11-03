#include "globewidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GlobeWidget w;
    w.show();
    return a.exec();
}
