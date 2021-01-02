#include "notify.h"
#include <qdebug.h>
#include <qcoreapplication.h>

Notify::Notify(QObject *parent) : QObject(parent)
{
    effect = new QSoundEffect(this);
}

void Notify::play(Notify::NotifySound type)
{
    static const notify_entry sounds[] =
    {
        {capture,   "click_in"},
        {sended,    "click_send"},
        {result,    "click_out"},
        {error,     "error"},
        {cancel,    "cancel"},
        {none,      nullptr},
    };

    const notify_entry* p = nullptr;
    for(const notify_entry& e : sounds){
        p = &e;
        if (p->type == type) break;
    }
    if (!p->name) return;

    QString file(":/media/%1.wav");
    auto url = QUrl::fromLocalFile(file.arg(p->name));
    effect->setSource(url);
    effect->play();
}
