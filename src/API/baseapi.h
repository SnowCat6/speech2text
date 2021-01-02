#ifndef BASEAPI_H
#define BASEAPI_H

#include <QObject>
#include "signals.h"

class BaseStreamAPI;
class QAudioInput;

//  Базовый класс для работы распознаванием речи
class BaseAPI : public QObject
{
    Q_OBJECT
public:
    using State = SpeechSignals::State;

public:
    explicit BaseAPI(QObject *parent = nullptr);
    ~BaseAPI();

public:
    SpeechSignals* events()const;
    //  Получить состояние процесса распознавания речи
    State state()const;
    //  Установить состояние процесса распознавания речи
    void state(State value);
    //  Получить локал текущий раскладки клавиатуры
    QString locale() const;

    void setDevice(BaseStreamAPI* pDevice);
    BaseStreamAPI* device()const;

public:
    virtual void start();
    virtual void stop();
    virtual void cancel();

protected:
    BaseStreamAPI* pSpeechDevice = nullptr;
    QAudioInput* pAudioInput = nullptr;
    SpeechSignals* speech_signals= nullptr;
    State current_state = State::stopped;
};

Q_DECLARE_METATYPE(BaseAPI::State)

#endif // BASEAPI_H
