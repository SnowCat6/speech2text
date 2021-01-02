#ifndef BASESTREAMAPI_H
#define BASESTREAMAPI_H

#include <QIODevice>
class QAudioFormat;
class SpeechSignals;

class BaseStreamAPI : public QIODevice
{
    Q_OBJECT

public:
    BaseStreamAPI(QObject *parent = nullptr);
    ~BaseStreamAPI();

    SpeechSignals *events()const;
    QString locale() const;

    QStringList& messages();

public:
    struct speechConfig_t{
        bool profanity_filter = false;
    };

protected:
    virtual void addMessage(const QString& message);

    virtual qint64 writeData(const char *data, qint64 len) override;
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 readLineData(char *data, qint64 maxlen) override;

public:
    virtual QAudioFormat getAudioFormat() const = 0;
    virtual qint64 size() const override;
    virtual bool reset() override;
    virtual bool canReadLine() const override;

protected:
    SpeechSignals* speech_events= nullptr;
    QVector<char> dataBuffer;
    QStringList readyMessages;
};

#endif // BASESTREAMAPI_H
