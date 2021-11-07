#ifndef GLOBEWIDGET_H
#define GLOBEWIDGET_H

#include "camera.h"

#include <QOpenGLWidget>
#include <QObject>

class GlobeWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GlobeWidget(QOpenGLWidget *parent = nullptr);
    ~GlobeWidget();

protected:
    void initializeGL();
    void paintGL();
    void keyPressEvent(QKeyEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private:
    GLuint _glBufferId;
    GLuint _glTextureID[2];
    float theta;
    Camera _camera;

private slots:
    void updateSunPosition();
};

#endif // GLOBEWIDGET_H
