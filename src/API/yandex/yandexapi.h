#ifndef YANDEXAPI_H
#define YANDEXAPI_H

#include <API/basestreamapi.h>

class QNetworkAccessManager;

//  Базовый класс с функциями доступа к сервисам яндекс
class YandexAPI : public BaseStreamAPI
{
    Q_OBJECT

    struct yandexSpeechConfig_t : public speechConfig_t
    {
        QString model;
        yandexSpeechConfig_t(): model("general"){}
    };

public:
    explicit YandexAPI(QObject *parent = nullptr);

public:
    void login(const QString &YandexToken, const QString &IAM_filder);
    void loginToken(const QString &YandexApiKey);

protected:
    QString YandexToken;
    QString IAM_auth;
    QString IAM_folder;

protected:
    yandexSpeechConfig_t yandexConfig;
    QNetworkAccessManager * net;

protected:
    QString yandexLocale()const;
    void updateIAM();

public:
    virtual QAudioFormat getAudioFormat() const override;
    virtual bool open(OpenMode mode) override;
};

#endif // YANDEXAPI_H
