#ifndef YANDEXBASE_H
#define YANDEXBASE_H

#include <QTimer>
#include "yandexapi.h"

class QNetworkReply;
//  Базовый класс сервиса для распознавания коротких аудиофайлов
class YandexSpeechV1 : public YandexAPI
{
    Q_OBJECT

    static constexpr auto max_size = 1*1024*1024;

public:
    explicit YandexSpeechV1(QObject *parent = nullptr);

public:

private:
    QTimer timer;
    QNetworkReply* reply = nullptr;

private:
    void sendSound();
    void closeSound(bool bFlush);
    void receiveSound();

    // QIODevice interface
public:
    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    virtual bool reset() override;
};

#endif // YANDEXBASE_H
