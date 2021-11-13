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

private:
    GLuint _glBufferId;
    GLuint _glTextureID[2];
    float theta, phi;
    int capturing;
    float sunHeight;

private slots:
    void updateSunPosition();
};

#endif // GLOBEWIDGET_H
