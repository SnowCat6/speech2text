#include "yandexspeechv1.h"

#include <qnetworkaccessmanager.h>
#include <qurlquery.h>
#include <qnetworkreply.h>

#include <qjsondocument.h>
#include <qjsonobject.h>

#include "../signals.h"

YandexSpeechV1::YandexSpeechV1(QObject *parent) : YandexAPI(parent)
{
    connect(&timer, &QTimer::timeout, this, &YandexSpeechV1::sendSound);
}

//  Запуск записи речи
bool YandexSpeechV1::open(QIODevice::OpenMode mode)
{
    if (!YandexAPI::open(mode))
        return false;

    //  Ограничения яндекса по времени файла
    timer.start(30*1000);

    return true;
}
//  Остановить запись и отправить на распознавание
void YandexSpeechV1::close()
{
    timer.stop();
    closeSound(true);
    YandexAPI::close();
}
//  Прекратить запись речи и отменить распознавание
bool YandexSpeechV1::reset()
{
    timer.stop();
    closeSound(false);
    return YandexAPI::reset();
}

void YandexSpeechV1::sendSound()
{
    if (!size()) return;
    if (reply) return;

    auto data = read(max_size);

    QUrl url("https://stt.api.cloud.yandex.net/speech/v1/stt:recognize");

    QUrlQuery query;
    query.addQueryItem("topic", yandexConfig.model);
    query.addQueryItem("profanityFilter", yandexConfig.profanity_filter?"true":"false");
    query.addQueryItem("lang", yandexLocale());
    query.addQueryItem("format", "lpcm");
    query.addQueryItem("sampleRateHertz", "8000");

    QNetworkRequest req;
    req.setRawHeader("Authorization", IAM_auth.toUtf8());
    if (!IAM_folder.isEmpty()) query.addQueryItem("folderId", IAM_folder);

    req.setHeader(QNetworkRequest::ContentTypeHeader, "audio/x-pcm;bit=16;rate=8000");
    req.setHeader(QNetworkRequest::ContentLengthHeader, data.size());

    url.setQuery(query);
    req.setUrl(url);

    reply = net->post(req, data);
    connect(reply, &QIODevice::readyRead, this, &YandexSpeechV1::receiveSound);
}

void YandexSpeechV1::closeSound(bool bFlush)
{
    if (bFlush) {
        sendSound();
    }else{
        if (reply){
            reply->close();
            reply->deleteLater();
            reply = nullptr;
        }
    }
}

void YandexSpeechV1::receiveSound()
{
    QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();
    reply = nullptr;

    QString result;
    result= json["result"].toString();
    if (!result.isEmpty())
        emit events()->onMessage(result);

    result = json["error_message"].toString();
    if (!result.isEmpty())
        emit events()->onError(result);
}
