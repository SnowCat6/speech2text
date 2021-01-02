#ifndef YANDEXSPEECHV2_H
#define YANDEXSPEECHV2_H

#include "yandexapi.h"
#include <yandex/cloud/ai/stt/v2/stt_service.grpc.pb.h>

//  Базовый класс сервисов для непрерывного распознавания голоса
class YandexSpeechV2 : public YandexAPI
{
    Q_OBJECT

    using ServicePtr = std::unique_ptr<::yandex::cloud::ai::stt::v2::SttService::Stub>;
    using RecognitionSpec = ::yandex::cloud::ai::stt::v2::RecognitionSpec;
    using StreamPtr = std::unique_ptr<::grpc::ClientReaderWriter< ::yandex::cloud::ai::stt::v2::StreamingRecognitionRequest, ::yandex::cloud::ai::stt::v2::StreamingRecognitionResponse>>;
    using StreamingRecognitionResponse = ::yandex::cloud::ai::stt::v2::StreamingRecognitionResponse;
    using AsyncStreamPtr = std::unique_ptr<::grpc::ClientAsyncReaderWriter< ::yandex::cloud::ai::stt::v2::StreamingRecognitionRequest, ::yandex::cloud::ai::stt::v2::StreamingRecognitionResponse>>;

    using ClientContext = grpc::ClientContext;
    using CompletionQueue = grpc::CompletionQueue;

    using time_point = std::chrono::system_clock::time_point;

    static constexpr auto CHUNK_SIZE = 4000;

public:
    explicit YandexSpeechV2(QObject *parent = nullptr);
    ~YandexSpeechV2();

public:

private:
    bool createSoundService();
    void createSoundInterface();
    void sendSound(qint64 min_size);
    void receiveSound(bool bReadSync);
    void closeSound(bool bFlushSound);

private:
    AsyncStreamPtr pStream;

    bool bReadOK = false;
    bool bReadedOK = false;
    StreamingRecognitionResponse response;
    time_point next_time_point;

private:
    CompletionQueue* query = nullptr;
    ServicePtr pService;

    // QIODevice interface
public:
    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    virtual bool reset() override;

protected:
    void flushSound();
};

#endif // YANDEXSPEECHV2_H
