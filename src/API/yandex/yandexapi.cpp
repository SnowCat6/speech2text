#include "yandexapi.h"

#include <qurlquery.h>

#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>

#include <qjsondocument.h>
#include <qjsonobject.h>

#include <qaudioformat.h>
#include "../signals.h"

YandexAPI::YandexAPI(QObject *parent) : BaseStreamAPI(parent)
{
    net = new QNetworkAccessManager(this);
}
//  Логин с помощью персонального ключа пользователя и каталога сервиса
void YandexAPI::login(const QString &YandexToken, const QString &IAM_filder)
{
    this->YandexToken = YandexToken;
    this->IAM_folder = IAM_filder;
    updateIAM();
}
//  Логин помощью токина апи ключа сервиса
void YandexAPI::loginToken(const QString &YandexApiKey)
{
    IAM_auth = "Api-Key " + YandexApiKey;
}

QString YandexAPI::yandexLocale() const
{
    QString l = locale();
    return l.replace("_", "-");
}

void YandexAPI::updateIAM()
{
    QJsonObject json;
    json["yandexPassportOauthToken"] = YandexToken;

    QByteArray data = QJsonDocument(json).toJson();
    QUrl url("https://iam.api.cloud.yandex.net/iam/v1/tokens");

    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setHeader(QNetworkRequest::ContentLengthHeader, data.size());

    QNetworkReply* reply = net->post(req, data);
    auto callback = [=]
    {
        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        reply->deleteLater();

        QString result;
        result = json["iamToken"].toString();
        if (!result.isEmpty()){
            IAM_auth = "Bearer " + result;
        }

        result = json["error_message"].toString();
        if (!result.isEmpty())
            emit events()->onError(result);
    };

    connect(reply, &QIODevice::readyRead, this, callback);
}

QAudioFormat YandexAPI::getAudioFormat() const
{
    QAudioFormat format;
    // set up the format you want, eg.
    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    return format;
}

bool YandexAPI::open(QIODevice::OpenMode mode)
{
    if (IAM_auth.isEmpty()) return false;
    return BaseStreamAPI::open(mode);
}
