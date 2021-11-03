#ifndef GLOBEWIDGET_H
#define GLOBEWIDGET_H

#include <QOpenGLWidget>
#include <QObject>

class GlobeWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GlobeWidget(QOpenGLWidget *parent = nullptr);
    ~GlobeWidget();

    void initializeGL();
    void paintGL();

private:
    GLuint _glBufferId, _glTextureID;
    float theta;

private slots:
    void updateSunPosition();
};

#endif // GLOBEWIDGET_H
