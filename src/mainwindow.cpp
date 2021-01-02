#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SYS/settings.h"

#include "SYS/notify.h"
#include <API/yandex/yandexspeechv1.h>
#include <API/yandex/yandexspeechv2.h>

#include "API/signals.h"

MainWindow::MainWindow(QWidget *parent) :
    SystemTools(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto ya = new YandexSpeechV1(this);
//    auto ya = new YandexSpeechV2(this);
    ya->loginToken("AQVNxRNca8X4GDHXSByIhrUOtd26ciQsIoZQoJid");

    speech = new BaseAPI();
    speech->setDevice(ya);

    connect(speech->events(), &SpeechSignals::onMessage, this, &MainWindow::onMessage);
    connect(speech->events(), &SpeechSignals::onMessagePreview, this, &MainWindow::onMessagePreview);
    connect(speech->events(), &SpeechSignals::onError, this, &MainWindow::onError);

    connect(speech->events(), &SpeechSignals::onStateChanged, this, &MainWindow::onStateChanged);
    connect(speech->events(), &SpeechSignals::onStateChanged, this, &MainWindow::updateUI);

    connect(this, &SystemTools::ononHotkey, this, &MainWindow::onHotkey);
    connect(this, &SystemTools::onError, this, &MainWindow::onError);

    QString name;
    name = hotkeyName(started);
    if (!name.isEmpty()) ui->start->setText(ui->start->text() + " (" + name + ")");
    name = hotkeyName(stopped);
    if (!name.isEmpty()) ui->stop->setText(ui->stop->text() + " (" + name + ")");
    name = hotkeyName(cancel);
    if (!name.isEmpty()) ui->cancel->setText(ui->cancel->text() + " (" + name + ")");

    connect(ui->chAutostart, &QAbstractButton::clicked, this, &MainWindow::fromForm);
    connect(ui->bCloseToTray, &QAbstractButton::clicked, this, &MainWindow::fromForm);
    connect(ui->bCopyToClipboard, &QAbstractButton::clicked, this, &MainWindow::fromForm);
    connect(ui->bPasteToWindow, &QAbstractButton::clicked, this, &MainWindow::fromForm);

    toForm();
    updateUI();
}

MainWindow::~MainWindow()
{
    speech->cancel();
    delete ui;
}

void MainWindow::updateUI()
{
    bool bStarted = speech->state() == SpeechSignals::started;
    bool bStopped = speech->state() == SpeechSignals::stopped;

    ui->start->setEnabled(bStopped);
    ui->stop->setEnabled(bStarted);
    ui->cancel->setEnabled(!bStopped);
}

void MainWindow::toForm()
{
    blockSignals(true);

    ui->chAutostart->setChecked(pSettings->bAutoStart);
    ui->bCloseToTray->setChecked(pSettings->bCloseToTray);
    ui->bCopyToClipboard->setChecked(pSettings->bCopyToClipboard);
    ui->bPasteToWindow->setChecked(pSettings->bPasteToWindow);

    blockSignals(false);
}

void MainWindow::fromForm() const
{
    pSettings->bAutoStart = ui->chAutostart->isChecked();
    pSettings->bCloseToTray = ui->bCloseToTray->isChecked();
    pSettings->bCopyToClipboard = ui->bCopyToClipboard->isChecked();
    pSettings->bPasteToWindow = ui->bPasteToWindow->isChecked();

    pSettings->save();
}

void MainWindow::onHotkey(SystemTools::Hotkey  id)
{
    switch (id) {
    case cancel:
        speech->cancel();
        break;
    case stopped:
        speech->stop();
        break;
    case started:
        speech->start();
        break;
    }
}

void MainWindow::on_start_clicked()
{
    speech->start();
}

void MainWindow::on_stop_clicked()
{
    speech->stop();
}

void MainWindow::onMessage(QString message)
{
    ui->message->setText(message);

    if (pSettings->bCopyToClipboard) copyText(message);
    if (pSettings->bPasteToWindow) sendText(message);

    notify->play(Notify::result);
}

void MainWindow::onMessagePreview(QString message)
{
    ui->message->setText(message);
}

void MainWindow::onError(QString error_message)
{
    QString html = QString("<span style='color:red'>%1</span>").arg(error_message);
    ui->message->setHtml(html);
    notify->play(Notify::error);
}

void MainWindow::on_cancel_clicked()
{
    speech->cancel();
}
