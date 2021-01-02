#include "signals.h"

SpeechSignals::SpeechSignals(QObject *parent)
    : QObject(parent)
{

}

void SpeechSignals::bind(SpeechSignals *events) const
{
    connect(this, &SpeechSignals::onMessage, events, &SpeechSignals::onMessage);
    connect(this, &SpeechSignals::onMessagePreview, events, &SpeechSignals::onMessagePreview);
    connect(this, &SpeechSignals::onError, events, &SpeechSignals::onError);
}
