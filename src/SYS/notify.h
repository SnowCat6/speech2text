#ifndef NOTIFY_H
#define NOTIFY_H

#include <QObject>

class QSoundEffect;

class Notify : public QObject
{
    Q_OBJECT
public:
    enum NotifySound
    {
        capture,    //  Capture sound started
        sended,     //  Sended to serverm wait result
        cancel,     //  Cancel
        error,      //  Any error
        result,     //  Result received
        none,
    };

public:
    explicit Notify(QObject *parent = nullptr);
    void play(NotifySound type);

signals:

private:
    QSoundEffect* effect;

    struct notify_entry{
        NotifySound type;
        const char* name;
    };
};

#endif // NOTIFY_H
