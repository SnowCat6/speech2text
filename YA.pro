QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
# Protobuff compile
CONFIG += object_parallel_to_source

BASEDIR=$${PWD}
Release:DESTDIR = release
Debug:DESTDIR = debug

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

#QT += multimedia

CONFIG += link_pkgconfig

win32 {
    LIBS += -lUser32
    RC_ICONS = $${PWD}/images/icon.ico
}

macx {
    # Enable pkg-config (pkg-config is disabled by default in the Qt package for mac)
    QT_CONFIG -= no-pkg-config
    # pkg-config location if your brew installation is standard
    PKG_CONFIG = /usr/local/bin/pkg-config
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
    RC_ICONS = $${PWD}/images/icon.icns
}

MODULES = \
    src

for(WORKPLACE_DIR, MODULES) {
    INCLUDEPATH += $${WORKPLACE_DIR}
    SOURCES     += $$files($${WORKPLACE_DIR}/*.c*,   true)
    HEADERS     += $$files($${WORKPLACE_DIR}/*.h*,   true)
    FORMS       += $$files($${WORKPLACE_DIR}/*.ui,   true)
    RESOURCES   += $$files($${WORKPLACE_DIR}/*.qrc,  true)
    include($$files($${WORKPLACE_DIR}/*.pro,  true))
}

RESOURCES += resources.qrc
DISTFILES += $$files(deploy/*.*)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#DEPLOY "CONFIG+=deploy"
if (deploy){
    win32:make_bundle.commands= "@$${PWD}/deploy/win_make_bundle.cmd" "$${OUT_PWD}/$${DESTDIR}" $${TARGET} "win.x64"
    macx:make_bundle.commands = @sh "$${PWD}/deploy/osx_make_bundle.sh" "$${OUT_PWD}/$${DESTDIR}" $${TARGET} "$${QMAKE_MACOSX_DEPLOYMENT_TARGET}"
}
!isEmpty(make_bundle.commands){
    message("Deploy $${TARGET}")
    QMAKE_EXTRA_TARGETS += make_bundle
    POST_TARGETDEPS += make_bundle
}
