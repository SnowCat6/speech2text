#include "basestreamapi.h"
#include "signals.h"
#include <qapplication.h>

BaseStreamAPI::BaseStreamAPI(QObject *parent)
    : QIODevice(parent)
{
    speech_events = new SpeechSignals(this);
    connect(events(), &SpeechSignals::onMessage, this, &BaseStreamAPI::addMessage);
}

BaseStreamAPI::~BaseStreamAPI()
{
}

SpeechSignals *BaseStreamAPI::events() const
{
    return speech_events;
}

QString BaseStreamAPI::locale() const
{
    return "ru-RU";
    QString lang = QApplication::inputMethod()->locale().name();
    return lang;
}

QStringList &BaseStreamAPI::messages()
{
    return readyMessages;
}

void BaseStreamAPI::addMessage(const QString &message)
{
    readyMessages.push_back(message);
}

qint64 BaseStreamAPI::writeData(const char *data, qint64 len)
{
    quint64 sz = dataBuffer.size();
    qint64 newSz = sz + len;

    dataBuffer.resize(newSz);
    memcpy(dataBuffer.begin()+sz, data, len);

    emit bytesWritten(len);

    return len;
}

qint64 BaseStreamAPI::readData(char *data, qint64 maxlen)
{
    qint64 len = qMin(maxlen, qint64(dataBuffer.size()));

    memcpy(data, dataBuffer.begin(), len);
    dataBuffer.erase(dataBuffer.begin(), dataBuffer.begin()+len);

    return len;
}

qint64 BaseStreamAPI::readLineData(char *data, qint64 maxlen)
{
    if (readyMessages.isEmpty()) return 0;

    auto line = readyMessages.first().toUtf8();
    readyMessages.pop_front();

    auto len = qMin(qint64(line.size()), maxlen);
    memcpy(data, line.data(), len);

    return 0;
}

qint64 BaseStreamAPI::size() const
{
    return dataBuffer.size();
}

bool BaseStreamAPI::reset()
{
    dataBuffer.clear();
    return QIODevice::reset();
}

bool BaseStreamAPI::canReadLine() const
{
    return !readyMessages.isEmpty();
}
