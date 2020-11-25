QT       += core gui widgets

TARGET = gameengine
TEMPLATE = app

SOURCES += main.cpp \
    scenegraph.cpp \
    scenegraphnode.cpp \
    transform.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp

HEADERS += \
    BasicIO.h \
    mainwidget.h \
    geometryengine.h \
    scenegraph.h \
    scenegraphnode.h \
    transform.h

RESOURCES += \
    obj.qrc \
    shaders.qrc \
    textures.qrc

# install
target.path = $$[YOUR_PATH]
INSTALLS += target

DISTFILES +=
