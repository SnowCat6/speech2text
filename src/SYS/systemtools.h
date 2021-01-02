#ifndef HOTKEYWINDOWS_H
#define HOTKEYWINDOWS_H

#include <qmainwindow.h>
#include <qsystemtrayicon.h>
#include <API/baseapi.h>

class Settings;
class Notify;

//  Настройка над главным окном QT выполняющим различные сервисные функции
class SystemTools : public QMainWindow
{
    Q_OBJECT

public:
    enum Hotkey{
        started,    //  Начать распознавание речи
        stopped,    //  Остановить распознавание речи и выдать результат
        cancel,     //  Прекратить распознавание речи
    };
public:
    explicit SystemTools(QWidget *parent);
    ~SystemTools();

    void copyText(const QString &message);
    void sendText(const QString &message);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;

    QString hotkeyName(Hotkey hotkey);

signals:
    //  Сигнал какой либо ошибки
    void onError(QString error_messgae);
    //  Нажата горячая клавиша
    void ononHotkey(Hotkey hoykey);

protected:
    void createTray();
    void registerHotkey();
    void pressKey(ushort keyCode);
    void onStateChanged(BaseAPI::State state);

    // QWidget interface
protected:
    void changeEvent(QEvent *) override;
    void closeEvent(QCloseEvent *event) override;

public:
    Notify* notify;
    Settings* pSettings;

private:
    QMenu* pTrayMenu;
    QSystemTrayIcon *trayIcon;
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    // QWidget interface
public slots:
    void setVisible(bool visible) override;
};

Q_DECLARE_METATYPE(SystemTools::Hotkey)

#endif // HOTKEYWINDOWS_H
