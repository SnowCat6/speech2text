#ifndef SPEECH_SIGNALS_H
#define SPEECH_SIGNALS_H

#include <qobject.h>

class SpeechSignals : public QObject
{
    Q_OBJECT
public:
    enum State{
        started,    //  Начало распознавания речи
        stopped,    //  Начало распознавания речи выдачи результата
        waiting,    //  Ожидание обработки распознавания речи
        cancelled,  //  Прекратить распознавание речи без выдачи результата
    };

public:
    SpeechSignals(QObject *parent = nullptr);
    void bind(SpeechSignals* events)const;

signals:
    //  События полностью распознанного звука
    void onMessage(QString messgae);
    void onMessagePreview(QString messgae);
    //  Ошибка в процессе работы
    void onError(QString error_messgae);
    //  Изменилось состояние работы процесса
    void onStateChanged(State state);
};

#endif // SIGNALS_H
