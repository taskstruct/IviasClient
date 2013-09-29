#ifndef QMLSETTINGSSINGLETON_H
#define QMLSETTINGSSINGLETON_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include <QtSql/QSqlDatabase>

class QMLSettingsSingleton : public QObject
{
    Q_OBJECT

    // shutdown time?, back to intro time(declarative view onMouse handling), show seatbelt warning,
    Q_PROPERTY(bool showSeatbeltWarningEnabled READ showSeatbeltWarningEnabled NOTIFY showSeatbeltWarningEnabledChanged)
    Q_PROPERTY(int seatbeltWarningDuration READ seatbeltWarningDuration NOTIFY seatbeltWarningDurationChanged)
    Q_PROPERTY(QStringList titles READ titles NOTIFY titlesChanged)

public:
    static QMLSettingsSingleton* instance(QObject *parent = 0);

    bool showSeatbeltWarningEnabled() const { return m_showSeatbeltWarningEnabled; }
    int seatbeltWarningDuration() const { return m_seatbeltWarningDuration; }
    QStringList titles() { return m_titles; }

    // not exported ot QML
    QUrl url() const { return m_url; }
    QString username() const { return m_username; }
    QString password() const { return m_password; }
    int port() const { return m_port; }
    int updateInterval() const { return m_updateInterval; }

    void fetchProjectId(QSqlDatabase &db);
    bool isInitialized() { return -1 != m_projectId;  }
    void refetchData();
signals:
    void projectChanged();
    void showSeatbeltWarningEnabledChanged();
    void seatbeltWarningDurationChanged();
    void titlesChanged();
    void updateIntervalChanged();

private:
    explicit QMLSettingsSingleton(QObject *parent = 0);

    static QMLSettingsSingleton* s_instance;

    int m_projectId;

    QUrl m_url;
    QString m_username;
    QString m_password;
    int m_port;

    bool m_showSeatbeltWarningEnabled;
    int m_seatbeltWarningDuration;
    int m_updateInterval;
    QStringList m_titles;
};

#endif // QMLSETTINGSSINGLETON_H
