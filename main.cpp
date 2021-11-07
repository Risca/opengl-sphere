#include "globewidget.h"

#include <QApplication>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QSurfaceFormat fmt;
    fmt.setRenderableType(QSurfaceFormat::OpenGLES);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication a(argc, argv);
    GlobeWidget w;
    w.show();
    return a.exec();
}
