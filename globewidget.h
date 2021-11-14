#ifndef GLOBEWIDGET_H
#define GLOBEWIDGET_H

#include <QDateTime>
#include <QOpenGLWidget>
#include <QObject>

class QDate;
class QTime;

class GlobeWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GlobeWidget(QWidget *parent = 0);
    ~GlobeWidget();

public slots:
    void setDate(const QDate& date);
    void setTime(int minutesSinceMidnight);
    void setTime(const QTime& time);

protected:
    void initializeGL();
    void paintGL();

private:
    GLuint _glBufferId;
    GLuint _glTextureID[2];
    float theta, phi;
    QDateTime dt;
};

#endif // GLOBEWIDGET_H
