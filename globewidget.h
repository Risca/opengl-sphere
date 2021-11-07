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
    GLuint _glBufferId;
    GLuint _glTextureID[2];
    float theta;

private slots:
    void updateSunPosition();
};

#endif // GLOBEWIDGET_H
