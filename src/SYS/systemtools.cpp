#include "systemtools.h"

#include "notify.h"
#include "settings.h"
#include <qmainwindow.h>

#include <QApplication>
#include <qclipboard.h>
#include <common/window_state_serialize.h>

#include <QCloseEvent>
#include <QSettings>
#include <qdir.h>
#include <qmenu.h>
#include <qtimer.h>

#ifdef Q_OS_WIN32
#include <QCloseEvent>
#include <qmenu.h>
#include <qtimer.h>
#include <windows.h>
#endif

SystemTools::SystemTools(QWidget *parent) : QMainWindow(parent)
{
    notify = new Notify(this);
    pSettings = new Settings(this);

    createTray();
    registerHotkey();

    QSettings qsettings;
    qsettings.beginGroup( "MainWIndow" );
    qsettings >> this;
    qsettings.endGroup();
}

SystemTools::~SystemTools()
{
    QSettings qsettings;
    qsettings.beginGroup( "MainWIndow" );
    qsettings << this;
    qsettings.endGroup();
}

void SystemTools::onStateChanged(BaseAPI::State state)
{
    QIcon icon(":/images/sound.png");
    switch (state)
    {
    case SpeechSignals::started:
        icon = QIcon(":/images/sound_on.png");
        notify->play(Notify::capture);
        break;
    case SpeechSignals::waiting:
        notify->play(Notify::sended);
        break;
    case SpeechSignals::cancelled:
        notify->play(Notify::cancel);
        break;
    default:
        break;
    }
    trayIcon->setIcon(icon);
}

void SystemTools::copyText(const QString &message)
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(message);
}

void SystemTools::sendText(const QString &message)
{
    std::wstring str = message.toStdWString();
    for(auto c: str)
    {
        pressKey(c);
    }
}

void SystemTools::pressKey(ushort keyCode)
{
#ifdef Q_OS_WIN32
    INPUT ip;

    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.wVk = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.ki.wScan = keyCode;
    ip.ki.dwFlags = KEYEVENTF_UNICODE;
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
#endif
}

void SystemTools::changeEvent(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::LanguageChange:
        break;
    case QEvent::WindowStateChange:
        {
            bool bMimimized = this->isMinimized();
            if (bMimimized) {
                QTimer::singleShot(0, this, &SystemTools::hide);
            }else{
                QTimer::singleShot(0, this, &SystemTools::show);
            }
            trayIcon->setVisible(bMimimized);
            break;
        }
    default:
        break;
    }

    QMainWindow::changeEvent(e);
}

void SystemTools::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        setWindowState(Qt::WindowActive);
        raise();
        break;
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        break;
    }
}

void SystemTools::setVisible(bool visible)
{
    trayIcon->setToolTip(windowTitle());
    QMainWindow::setVisible(visible);
}

void SystemTools::closeEvent(QCloseEvent *event)
{
    if (!pSettings->bCloseToTray)
        return;

#ifdef Q_OS_MACOS
    if (!event->spontaneous() || !isVisible()) {
        return;
    }
#endif
    if (!trayIcon->isVisible()) {
        showMinimized();
        event->ignore();
    }
}

bool SystemTools::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);

#ifdef Q_OS_WIN32
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        int id = msg->wParam;
        emit ononHotkey(Hotkey(id));
        return false;
    }
#endif
    return false;
}

QString SystemTools::hotkeyName(SystemTools::Hotkey hotkey)
{
#ifdef Q_OS_WIN32
    switch (hotkey) {
    case started:
        return "Ctrl+WIN";
    case stopped:
        return "Ctrl";
    case cancel:
        return "ESC";
    }
#endif
    return QString();
}

void SystemTools::createTray()
{
    pTrayMenu = new QMenu(this);

    QAction* pAction;
    pAction = pTrayMenu->addAction("Exit");
    connect(pAction, &QAction::triggered, this, &SystemTools::close);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(pTrayMenu);
    onStateChanged(SpeechSignals::stopped);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &SystemTools::iconActivated);
}

void SystemTools::registerHotkey()
{
#ifdef Q_OS_WIN32
    auto hWnd = HWND(winId());
    if (!::RegisterHotKey(hWnd, started, MOD_WIN | MOD_CONTROL, 0)){
        emit onError("Can't register hotkey WIN+CTR");
    }
    if (!::RegisterHotKey(hWnd, stopped, MOD_CONTROL, 0)){
        emit onError("Can't register hotkey CTR");
    }
/*
    if (!::RegisterHotKey(hWnd, cancel, 0, VK_ESCAPE)){
        emit onError("Can't register hotkey ESC");
    }
*/
#endif
}
