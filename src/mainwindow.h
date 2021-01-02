#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "SYS/systemtools.h"

class BaseAPI;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public SystemTools
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void updateUI();

    void toForm();
    void fromForm() const;

protected:
    void onHotkey(SystemTools::Hotkey id);
    void onMessage(QString message);
    void onMessagePreview(QString message);
    void onError(QString error_message);

protected slots:
    void on_start_clicked();
    void on_stop_clicked();

private slots:

    void on_cancel_clicked();

private:
    Ui::MainWindow *ui;
    BaseAPI* speech;
};
#endif // MAINWINDOW_H
