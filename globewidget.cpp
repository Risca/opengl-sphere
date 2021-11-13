#include "globewidget.h"

#include "primitives/shapegenerator.h"

#include <algorithm>
#include <glm/gtx/transform.hpp>
#include <QDebug>
#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QVector>

using glm::vec3;
using glm::vec4;
using glm::mat4;

namespace {

const double EARTH_TILT_ANGLE = 23.43645 * glm::pi<double>() / 180.0; // radians
Q_GLOBAL_STATIC_WITH_ARGS(const vec3, EARTH_TILT, (sin(EARTH_TILT_ANGLE), cos(EARTH_TILT_ANGLE), 0.0f));

std::string readShaderCode(const QString& filename)
{
    QFile file(filename);
    if (file.open(QFile::ReadOnly))
        return file.readAll().toStdString();
    return "";
}

template<typename F1, typename F2>
bool CheckStatus(GLuint objectID, F1 objectPropertyGetterFunc, F2 getInfoLogFunc, GLenum statusType)
{
    GLint status;
    glGetShaderiv(objectID, statusType, &status);
    if (status != GL_TRUE) {
        GLint infoLogLength;
        objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (!infoLogLength) {
            qDebug() << "Status not OK, but no log line";
            return false;
        }

        QVector<GLchar> buffer;
        buffer.resize(infoLogLength);
        GLsizei bufferSize;
        getInfoLogFunc(objectID, infoLogLength, &bufferSize, &buffer[0]);
        qDebug().noquote() << QString(&buffer[0]);
        return false;
    }
    return true;
}

bool CheckShaderStatus(GLuint shaderID)
{
    return CheckStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool CheckProgramStatus(GLuint programID)
{
    return CheckStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

struct ScopedShader
{
    explicit ScopedShader(GLenum type) :
        _shader(glCreateShader(type)) { /* empty */ }
    ~ScopedShader() {
        glDeleteShader(_shader);
    }
    operator GLuint() { return _shader; }
private:
    GLuint _shader;
};

GLuint g_programID;
void installShaders()
{
    std::string shaderCode;
    ScopedShader vertexShaderID(GL_VERTEX_SHADER);
    ScopedShader fragmentShaderID(GL_FRAGMENT_SHADER);

    const GLchar* adapter[1];
    shaderCode = readShaderCode(":/vertex_shader.glsl");
    adapter[0] = shaderCode.c_str();
    glShaderSource(vertexShaderID, 1, adapter, 0);

    shaderCode = readShaderCode(":/fragment_shader.glsl");
    adapter[0] = shaderCode.c_str();
    glShaderSource(fragmentShaderID, 1, adapter, 0);

    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);
    if (!CheckShaderStatus(vertexShaderID) || !CheckShaderStatus(fragmentShaderID))
        return;

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);

    // Need to bind attributes before linking
    glBindAttribLocation(programID, 0, "vertexPositionModel");
    glBindAttribLocation(programID, 1, "vertexColor");
    glBindAttribLocation(programID, 2, "normalModel");
    glBindAttribLocation(programID, 3, "textureCoordinateAttribute");

    glLinkProgram(programID);

    if (!CheckProgramStatus(programID))
        return;

    g_programID = programID;
    glUseProgram(programID);
}

GLint g_numIndices;
GLsizeiptr g_indexOffset;
void sendDataToOpenGL(GLuint& bufferID, GLuint textureID[])
{
    ShapeData shape = ShapeGenerator::makeSphere(50);

    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize() + shape.indexBufferSize(), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, shape.vertexBufferSize(), &shape.vertices[0]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, shape.vertexBufferSize(), shape.indexBufferSize(), &shape.indices[0]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, shape.vertexStride(), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, shape.vertexStride(), (void*)(sizeof(GLfloat) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, shape.vertexStride(), (void*)(sizeof(GLfloat) * 6));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, shape.vertexStride(), (void*)(sizeof(GLfloat) * 9));

    g_numIndices = shape.indices.count();
    g_indexOffset = shape.vertexBufferSize();

    glGenTextures(2, textureID);
    const QString filename[] = {
        ":/blue_marble.jpg",
        ":/black_marble.jpg",
    };
    for (int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, textureID[i]);
        QImage img = QImage(filename[i]).convertToFormat(QImage::Format_RGBX8888).mirrored(false, true);
        if (img.isNull()) {
            qWarning() << "Failed to load texture" << filename[i];
            return;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.constBits());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
}

} // anonymous namespace

GlobeWidget::GlobeWidget(QOpenGLWidget *parent) : QOpenGLWidget(parent), _glBufferId(0), theta(0.0f), phi(0.0f), capturing(false), sunHeight(1.0f)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GlobeWidget::updateSunPosition);
    timer->setInterval(20);
    timer->setSingleShot(false);
    timer->start();
}

GlobeWidget::~GlobeWidget()
{
    glUseProgram(0);
    glDeleteProgram(g_programID);
}

void GlobeWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // removes back side of shapes
    sendDataToOpenGL(_glBufferId, _glTextureID);
    installShaders();
    setMouseTracking(true);
}

template <typename T>
T deg2rad(T degrees)
{
    return degrees * glm::pi<T>() / T(180);
}

void GlobeWidget::paintGL()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    const float r = 10.0f;
    const float _sunHeight = sunHeight + cos(phi / 2.0f); // Move sun higher on winter to get approximately correct polar lights
    mat4 projectionMatrix = glm::perspective(deg2rad(60.0f), ((float)width()/height()), 0.1f, 10.0f);
    const vec3 earthPositionWorld = glm::rotate(mat4(1.0f), phi, vec3(0.0f, 1.0f, 0.0f)) * vec4(r, 0.0f, 0.0f, 1.0f);
    mat4 earthTranslationMatrix = glm::translate(mat4(1.0f), earthPositionWorld);
    mat4 earthRotationMatrix = glm::rotate(mat4(1.0f), theta, *EARTH_TILT) * glm::rotate(mat4(1.0f), static_cast<float>(EARTH_TILT_ANGLE), vec3(0.0f, 0.0f, -1.0f));
    mat4 modelToWorldMatrix = earthTranslationMatrix * earthRotationMatrix;

    GLint modelToWorldMatrixUniformLocation = glGetUniformLocation(g_programID, "modelToWorldMatrix");
    glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
    GLint projectionMatrixUniformLocation = glGetUniformLocation(g_programID, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    GLint ambientLightUniformLocation = glGetUniformLocation(g_programID, "ambientLight");
    glUniform4f(ambientLightUniformLocation, 0.05f, 0.05f, 0.1f, 1.0f);
    GLint sunColorUniformLocation = glGetUniformLocation(g_programID, "sunColor");
    glUniform4f(sunColorUniformLocation, 1.0f, 0.6f, 0.2f, 1.0f);

    const vec4 eyePositionModel =
            glm::rotate(mat4(1.0f), deg2rad(100.0f) + theta, *EARTH_TILT) *                             // follow earth rotation
            glm::rotate(mat4(1.0f), deg2rad(-60.0f), glm::cross(*EARTH_TILT, vec3(0.0f, 0.0f, 1.0f))) * // look more north
            vec4(0.0f, 0.0f, 3.0f, 1.0f);
    const vec3 eyePositionWorld = earthTranslationMatrix * eyePositionModel;
    GLint eyePositionWorldUniformLocation = glGetUniformLocation(g_programID, "eyePositionWorld");
    glUniform3fv(eyePositionWorldUniformLocation, 1, &eyePositionWorld[0]);

    mat4 worldToViewMatrix = glm::lookAt(eyePositionWorld, earthPositionWorld, *EARTH_TILT);
    //mat4 worldToViewMatrix = glm::lookAt(eyePositionWorld, earthPositionWorld, vec3(0.0f, 1.0f, 0.0f));
    GLint worldToViewMatrixUniformLocation = glGetUniformLocation(g_programID, "worldToViewMatrix");
    glUniformMatrix4fv(worldToViewMatrixUniformLocation, 1, GL_FALSE, &worldToViewMatrix[0][0]);

    const vec3 sunPositionWorld = vec3(0.0f, _sunHeight, 0.0f);
    GLint sunPositionUniformLocation = glGetUniformLocation(g_programID, "sunPositionWorld");
    glUniform3fv(sunPositionUniformLocation, 1, &sunPositionWorld[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _glTextureID[0]);
    GLint dayTextureHandleUniformLocation = glGetUniformLocation(g_programID, "dayTextureHandle");
    glUniform1i(dayTextureHandleUniformLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _glTextureID[1]);
    GLint nightTextureHandleUniformLocation = glGetUniformLocation(g_programID, "nightTextureHandle");
    glUniform1i(nightTextureHandleUniformLocation, 1);

    glDrawElements(GL_TRIANGLES, g_numIndices, GL_UNSIGNED_SHORT, (void*)g_indexOffset);
    if (capturing) {
        QImage img = grabFramebuffer();
        img.save(QString("capture_%1.jpg").arg(capturing, 3, 10, QChar('0')));
    }
}

void GlobeWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_C:
        capturing = 360 + 1;
        break;
    case Qt::Key_Q:
        this->close();
        break;
    case Qt::Key_P:
        qDebug() << "Sun height: " << sunHeight;
        break;
    }
}

void GlobeWidget::updateSunPosition()
{
    theta += deg2rad(1.0f);
    if (theta > 2 * glm::pi<float>()) {
        theta = 0.0f;
        phi += deg2rad(1.0f);
    }
    if (capturing)
        --capturing;
    repaint();
}
