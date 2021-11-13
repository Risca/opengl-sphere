QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    globewidget.cpp \
    main.cpp \
    primitives/shapegenerator.cpp

HEADERS += \
    globewidget.h \
    primitives/shapedata.h \
    primitives/shapegenerator.h \
    primitives/vertex.h

FORMS +=

DEFINES += GL_GLEXT_PROTOTYPES

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    fragment_shader.glsl \
    res/earthlights10k.jpg \
    res/earthlights1k.jpg \
    res/earthlights2k.jpg \
    res/earthlights4k.jpg \
    res/earthmap10k.jpg \
    res/earthmap1k.jpg \
    res/earthmap2k.jpg \
    res/earthmap4k.jpg \
    vertex_shader.glsl

RESOURCES += \
    resources.qrc
