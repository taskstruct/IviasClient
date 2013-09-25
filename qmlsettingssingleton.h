#ifndef QMLSETTINGSSINGLETON_H
#define QMLSETTINGSSINGLETON_H

#include <QObject>
#include <QStringList>

class QMLSettingsSingleton : public QObject
{
    Q_OBJECT

    // shutdown time?, back to intro time(declarative view onMouse handling), show seatbelt warning,
    Q_PROPERTY(bool showSeatbeltWarningEnabled READ showSeatbeltWarningEnabled NOTIFY showSeatbeltWarningEnabledChanged)
    Q_PROPERTY(int seatbeltWarningDuration READ seatbeltWarningDuration NOTIFY seatbeltWarningDurationChanged)
    Q_PROPERTY(QStringList titles READ titles NOTIFY titlesChanged)

public:
    explicit QMLSettingsSingleton(QObject *parent = 0);

    bool showSeatbeltWarningEnabled() const { return m_showSeatbeltWarningEnabled; }
    int seatbeltWarningDuration() const { return m_seatbeltWarningDuration; }
    QStringList titles() { return {"Page 1", "Page 2", "Page 3", "Page 4", "Page 5", "Page 6"}; }

    // not exported ot QML
    int updateInterval() const { return m_updateInterval; }

    void init();
    bool isInitialized() { return -1 != m_projectId;  }
    void refetchData();
signals:
    void showSeatbeltWarningEnabledChanged();
    void seatbeltWarningDurationChanged();
    void titlesChanged();
    void updateIntervalChanged();

private:
    int m_projectId;

    bool m_showSeatbeltWarningEnabled;
    int m_seatbeltWarningDuration;
    int m_updateInterval;
};

#endif // QMLSETTINGSSINGLETON_H
