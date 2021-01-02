#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

class Settings : public QObject
{
    Q_OBJECT
    static constexpr auto APPLICATION_NAME = "Speech";

public:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    void load();
    void save()const;

    bool isAutostart()const;
    void setAutostart(bool bAutoStart) const;

public:
    bool bAutoStart = false;
    bool bCloseToTray = true;
    bool bCopyToClipboard = false;
    bool bPasteToWindow = true;

signals:

};

#endif // SETTINGS_H
