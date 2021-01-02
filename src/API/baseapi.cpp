#include "baseapi.h"
#include "basestreamapi.h"

#include <qapplication.h>
#include <qlocale.h>

BaseAPI::BaseAPI(QObject *parent) : QObject(parent)
{
    speech_signals = new SpeechSignals(this);
}

BaseAPI::~BaseAPI()
{
    if (pSpeechDevice)
    {
        pAudioInput->stop();
        pAudioInput->deleteLater();
        pSpeechDevice->deleteLater();
    }
}

SpeechSignals *BaseAPI::events() const
{
    return speech_signals;
}

BaseAPI::State BaseAPI::state() const
{
    return current_state;
}

void BaseAPI::state(BaseAPI::State value)
{
    if (value == State::cancelled) {
        current_state = State::stopped;
    }else{
        current_state = value;
    }
    emit events()->onStateChanged(value);
}

void BaseAPI::setDevice(BaseStreamAPI *pDevice)
{
    if (pSpeechDevice)
    {
        pAudioInput->stop();
        pAudioInput->deleteLater();
        pSpeechDevice->deleteLater();
    }
    pSpeechDevice = pDevice;
    pSpeechDevice->events()->bind(events());

    auto format = pSpeechDevice->getAudioFormat();
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        emit events()->onError("default format not supported try to use nearest");
        format = info.nearestFormat(format);
    }

    pAudioInput = new QAudioInput(format, this);
}

BaseStreamAPI *BaseAPI::device() const
{
    return pSpeechDevice;
}

void BaseAPI::start()
{
    if (!pSpeechDevice) return;

    state(State::started);
    if (pSpeechDevice->isOpen()) pSpeechDevice->reset();
    pSpeechDevice->open(QIODevice::ReadWrite);
    pAudioInput->start(pSpeechDevice);
}

void BaseAPI::stop()
{
    if (!pSpeechDevice) return;
    if (!pSpeechDevice->isOpen()) return;

    state(State::waiting);
    pAudioInput->suspend();
    pAudioInput->stop();
    pSpeechDevice->close();

    state(State::stopped);
}

void BaseAPI::cancel()
{
    if (!pSpeechDevice) return;
    if (!pSpeechDevice->isOpen()) return;

    state(State::cancelled);
    pAudioInput->stop();
    if (pSpeechDevice->isOpen()) pSpeechDevice->reset();
    pSpeechDevice->close();
}
