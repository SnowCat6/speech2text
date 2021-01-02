#include "window_state_serialize.h"

#include <qsettings.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qmainwindow.h>
#include <qaction.h>

toSettings(QCheckBox)
{
    proto.setValue(node->objectName(), node->isChecked());
    return proto;
}
fromSettings(QCheckBox)
{
    auto value = proto.value(node->objectName());
    if (value.isValid()) node->setChecked(value.toBool());
    return proto;
}

toSettings(QLineEdit)
{
    proto.setValue(node->objectName(), node->text());
    return proto;
}
fromSettings(QLineEdit)
{
    auto value = proto.value(node->objectName());
    if (value.isValid()) node->setText(value.toString());
    return proto;
}

toSettings(QComboBox)
{
    proto.setValue(node->objectName(), node->currentIndex());
    return proto;
}
fromSettings(QComboBox)
{
    auto value = proto.value(node->objectName());
    if (value.isValid()) node->setCurrentIndex(value.toInt());
    return proto;
}

toSettings(QAction)
{
    proto.setValue(node->objectName(), node->isChecked());
    return proto;
}

fromSettings(QAction)
{
    auto value = proto.value(node->objectName());
    if (value.isValid() && value.toBool()) node->triggered(value.toBool());
    return proto;
}

toSettings(QMainWindow)
{
    auto geometry = node->saveGeometry();
    if (geometry.size())
    {
        proto.setValue( "geometry", geometry );
        proto.setValue( "savestate", node->saveState() );
        proto.setValue( "minimized", node->isMinimized());
        proto.setValue( "maximized", node->isMaximized());
        if (!node->isMaximized()) {
            proto.setValue( "pos", node->pos());
            proto.setValue( "size", node->size());
        }
    }
    return proto;
}

fromSettings(QMainWindow)
{
    node->restoreGeometry(proto.value( "geometry", node->saveGeometry() ).toByteArray());
    node->restoreState(proto.value( "savestate", node->saveState() ).toByteArray());
    node->move(proto.value( "pos", node->pos() ).toPoint());
    node->resize(proto.value( "size", node->size() ).toSize());
    if (proto.value( "maximized", node->isMaximized() ).toBool()){
        node->showMaximized();
    }
    if (proto.value("minimized", node->isMinimized() ).toBool()){
        node->showMinimized();
    }

    return proto;
}
