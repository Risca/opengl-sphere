#include "globewidget.h"

#include "primitives/shapegenerator.h"

#include <algorithm>
#include <glm/gtx/transform.hpp>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QVector>

using glm::vec3;
using glm::vec4;
using glm::mat4;

namespace {

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

GlobeWidget::GlobeWidget(QOpenGLWidget *parent) : QOpenGLWidget(parent), _glBufferId(0), theta(0.0f)
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
}

void GlobeWidget::paintGL()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    mat4 projectionMatrix = glm::perspective(60.0f * glm::pi<float>() / 180.0f, ((float)width()/height()), 0.1f, 10.0f);
    mat4 translationMatrix = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, -3.0f));
    mat4 modelToWorldMatrix = glm::rotate(translationMatrix, (0.0f * glm::pi<GLfloat>() ) / 180.0f, vec3(0.0f, 1.0f, 0.0f));

    GLint modelToWorldMatrixUniformLocation = glGetUniformLocation(g_programID, "modelToWorldMatrix");
    glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &modelToWorldMatrix[0][0]);
    GLint projectionMatrixUniformLocation = glGetUniformLocation(g_programID, "projectionMatrix");
    glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

    GLint ambientLightUniformLocation = glGetUniformLocation(g_programID, "ambientLight");
    glUniform4f(ambientLightUniformLocation, 0.05f, 0.05f, 0.1f, 1.0f);

    const float r = 10.0f;
    const vec4 sunPositionModel = glm::rotate(-theta, vec3(-0.3f, 1.0f, 0.0f)) * vec4(0.0f, 0.0f, r, 1.0f);
    const vec4 sunPositionWorld = translationMatrix * sunPositionModel;

    GLint sunPositionUniformLocation = glGetUniformLocation(g_programID, "sunPositionWorld");
    glUniform3fv(sunPositionUniformLocation, 1, &sunPositionWorld[0]);
    GLint sunColorUniformLocation = glGetUniformLocation(g_programID, "sunColor");
    glUniform4f(sunColorUniformLocation, 1.0f, 0.6f, 0.2f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _glTextureID[0]);
    GLint dayTextureHandleUniformLocation = glGetUniformLocation(g_programID, "dayTextureHandle");
    glUniform1i(dayTextureHandleUniformLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _glTextureID[1]);
    GLint nightTextureHandleUniformLocation = glGetUniformLocation(g_programID, "nightTextureHandle");
    glUniform1i(nightTextureHandleUniformLocation, 1);

    glDrawElements(GL_TRIANGLES, g_numIndices, GL_UNSIGNED_SHORT, (void*)g_indexOffset);
}

void GlobeWidget::updateSunPosition()
{
    theta += 1 * glm::pi<float>() / 180.0f;
    if (theta > 2 * glm::pi<float>()) {
        theta = 0.0f;
    }
    repaint();
}
