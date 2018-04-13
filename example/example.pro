#note: please make sure that libtiled, tiled-qml-plugin and qml-box2d are built with the same Qt version, otherwise things may crash
isEmpty(LIBTILED_LIBDIR) {
  error(Set LIBTILED_LIBDIR to the directory containing the libtiled.so.x.y.z)
}

DEFINES += TILED_QUICK_PLUGIN_DIR=\\\"$$PWD/../qml\\\"

#todo remove local stuff
DEFINES += BOX2D_QUICK_PLUGIN_DIR=\\\"$$clean_path($$OUT_PWD/../3rdparty/qml-box2d)\\\"
isEmpty(BOX2D_QUICK_PLUGIN_DIR) {
  #if qml-box2d is installed alongside your qt version you can ignore this warning
  warning(Set BOX2D_QUICK_PLUGIN_DIR to the directory containing the libBox2D.so qml plugin)
}
LIBS += -L$${LIBTILED_LIBDIR}

TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp

RESOURCES += qml.qrc

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += "../3rdparty/tiled/src"

DEFINES += EXAMPLE_DIR=\\\"$$clean_path($$PWD)\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
