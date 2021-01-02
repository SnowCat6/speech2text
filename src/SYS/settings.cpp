#include "settings.h"
#include <qsettings.h>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qdir.h>

Settings::Settings(QObject *parent) : QObject(parent)
{
    load();
}

Settings::~Settings()
{
    save();
}

void Settings::load()
{
    QSettings qsettings;
    qsettings.beginGroup( "settings" );

    bAutoStart = qsettings.value("autostart", bAutoStart).toBool();
    bCloseToTray = qsettings.value("closeToTray", bCloseToTray).toBool();
    bCopyToClipboard = qsettings.value("copyToClipboard", bCopyToClipboard).toBool();
    bPasteToWindow = qsettings.value("pasteToWindow", bPasteToWindow).toBool();

    qsettings.endGroup();

    bAutoStart = isAutostart();
    if (bAutoStart) setAutostart(bAutoStart);
}

void Settings::save() const
{
    QSettings qsettings;
    qsettings.beginGroup( "settings" );

    qsettings.setValue("autostart", bAutoStart);
    qsettings.setValue("closeToTray", bCloseToTray);
    qsettings.setValue("copyToClipboard", bCopyToClipboard);
    qsettings.setValue("pasteToWindow", bPasteToWindow);

    qsettings.endGroup();

    setAutostart(bAutoStart);
}

bool Settings::isAutostart() const
{
#ifdef Q_OS_WIN32
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    return settings.value(APPLICATION_NAME).toString().isEmpty() == false;
#endif
    return false;
}

void Settings::setAutostart(bool bAutoStart)const
{
#ifdef Q_OS_WIN32
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (bAutoStart){
        settings.setValue(APPLICATION_NAME, QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        settings.sync();
    }else{
        settings.remove(APPLICATION_NAME);
    }
#endif
}
