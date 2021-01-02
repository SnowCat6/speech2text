#include "yandexspeechv2.h"

#include <yandex/cloud/ai/stt/v2/stt_service.grpc.pb.h>
#include <yandex/cloud/ai/stt/v2/stt_service.pb.h>
#include <grpcpp/impl/codegen/client_context_impl.h>
#include <grpcpp/grpcpp.h>

#include <QDebug>
#include <qfile.h>

#include "../signals.h"

using namespace ::grpc;
using namespace ::yandex::cloud::ai::stt::v2;

YandexSpeechV2::YandexSpeechV2(QObject *parent)
    : YandexAPI(parent)
{
    createSoundService();
    connect(this, &QIODevice::bytesWritten, this, &YandexSpeechV2::flushSound);
}

YandexSpeechV2::~YandexSpeechV2()
{
    pStream = nullptr;
    if (query) delete query;
}

bool YandexSpeechV2::createSoundService()
{
    assert(!pService);
    //  https://github.com/grpc/grpc/blob/master/etc/roots.pem
    const static auto fileName = ":/external/roots.pem";
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Error read ssl certificate";
        emit events()->onError("Error read ssl certificate");
        return false;
    }

    grpc::SslCredentialsOptions opt;
    opt.pem_root_certs  = file.readAll().toStdString();

    auto ssl    = grpc::SslCredentials(opt);
    auto channel= CreateChannel("stt.api.cloud.yandex.net:443", ssl);
    pService    = SttService::NewStub(channel);

    return true;
}

void YandexSpeechV2::createSoundInterface()
{
    assert(!pStream);

    query = new CompletionQueue;

    next_time_point = std::chrono::system_clock::now() + std::chrono::duration<long>(1);
    bReadOK =false;
    bReadedOK =false;

//    qInfo() << "Data length" << data.length();
    auto pYandexSpec = new RecognitionSpec;
    pYandexSpec->set_language_code(yandexLocale().toStdString());
    pYandexSpec->set_profanity_filter(yandexConfig.profanity_filter);
    pYandexSpec->set_model(yandexConfig.model.toStdString());
    pYandexSpec->set_partial_results(true);
    pYandexSpec->set_audio_encoding(RecognitionSpec_AudioEncoding_LINEAR16_PCM);
    pYandexSpec->set_sample_rate_hertz(8000);

    auto pCfg = new RecognitionConfig;
    pCfg->set_allocated_specification(pYandexSpec);
    if (!IAM_folder.isEmpty()) pCfg->set_folder_id(IAM_folder.toStdString());

    StreamingRecognitionRequest req;
    req.set_allocated_config(pCfg);

    auto context = new ClientContext;
    context->AddMetadata("authorization", IAM_auth.toStdString());

    bool ok = false; void* tag;
    pStream = pService->AsyncStreamingRecognize(context, query, this);
    query->Next(&tag, &ok);

    pStream->Write(req, this);
    query->Next(&tag, &ok);
}

void YandexSpeechV2::sendSound(qint64 min_size)
{
    bool ok = false; void* tag;
    while(size() >= min_size)
    {
        QByteArray data = read(CHUNK_SIZE);

        StreamingRecognitionRequest req;
        req.set_audio_content(data.toStdString());

        pStream->Write(req, this);
        query->Next(&tag, &ok);
    }
}

void YandexSpeechV2::receiveSound(bool bReadSync)
{
    if (!bReadedOK && bReadSync)
        return;
    if (!bReadSync && std::chrono::system_clock::now() < next_time_point)
        return;

    bool ok = false; void* tag;

    if (!bReadOK)
    {
        bReadOK = true;
        pStream->Read(&response, &bReadOK);
    }

    if (bReadSync)
    {
        do{
            if (!query->Next(&tag, &ok)) break;
        }while(tag != &bReadOK);
    }else{
        if (!query->AsyncNext(&tag, &ok, next_time_point)) return;
        if (tag != &bReadOK) return;
    }

    next_time_point = std::chrono::system_clock::now() + std::chrono::milliseconds(250);
    bReadOK = false;
    bReadedOK = true;

    if (!response.chunks_size()) return;

    auto ch = response.chunks(0);
    if (!ch.alternatives_size()) return;

    auto text = ch.alternatives(0).text();
    if (text.empty()) return;

    if (bReadSync || ch.final())
    {
        emit events()->onMessage(QString::fromStdString(text));
    }else{
        emit events()->onMessagePreview(QString::fromStdString(text));
    }
/*
    qDebug() << (ch.final()?"Final: ":"")
            << "size: " << response.chunks_size()
             << ", " << text.data();
*/
}

void YandexSpeechV2::closeSound(bool bFlushSound)
{
    if (!pStream)
        return;

    if (bFlushSound){
        sendSound(1);
        receiveSound(true);
        pStream->WritesDone(this);
    }else{
        pStream->WritesDone(this);
    }

    bool ok = false; void* tag;

    Status status;;
    pStream->Finish(&status, this);
    query->Next(&tag, &ok);

    query->Shutdown();
    delete query;
    query = nullptr;

    pStream = nullptr;
}

bool YandexSpeechV2::open(QIODevice::OpenMode mode)
{
    if (!YandexAPI::open(mode))
        return false;

    createSoundInterface();

    return true;
}

void YandexSpeechV2::close()
{
    closeSound(true);
    YandexAPI::close();
}

bool YandexSpeechV2::reset()
{
    closeSound(false);
    return YandexAPI::reset();
}

void YandexSpeechV2::flushSound()
{
    sendSound(CHUNK_SIZE);
    receiveSound(false);
}
