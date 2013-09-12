#ifndef QMLSETTINGSSINGLETON_H
#define QMLSETTINGSSINGLETON_H

#include <QObject>
#include <QStringList>

class QMLSettingsSingleton : public QObject
{
    Q_OBJECT

    // shutdown time?, back to intro time(declarative view onMouse handling), show seatbelt warning,
    Q_PROPERTY(bool showSeatbeltWarningEnabled READ showSeatbeltWarningEnabled NOTIFY showSeatbeltWarningEnabledChanged)
    Q_PROPERTY(QStringList titles READ titles NOTIFY titlesChanged)

public:
    explicit QMLSettingsSingleton(QObject *parent = 0);

    bool showSeatbeltWarningEnabled() const { return true; }
    QStringList titles() { return {"Page 1", "Page 2", "Page 3", "Page 4", "Page 5", "Page 6"}; }

signals:
    void showSeatbeltWarningEnabledChanged();
    void titlesChanged();
};

#endif // QMLSETTINGSSINGLETON_H
