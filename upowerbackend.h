#ifndef UPOWERBACKEND_H
#define UPOWERBACKEND_H

#include <QtDBus/QDBusInterface>

#include "powermanager.h"

class QDBusPendingCallWatcher;

class UPowerBackend: public PowerBackendIface
{
    Q_OBJECT

public:
    UPowerBackend( QObject *parent = 0 );
    ~UPowerBackend() {}

    bool isOnBattery() const { return m_isOnBatt; }

private slots:
    void onEnumerateDevicesFinished(QDBusPendingCallWatcher *watcher);
    void onPropertyChanged();
    void onPercentageFinished(QDBusPendingCallWatcher *watcher);

private:
    void setupBattery();
    void updatePercentage();

    QString m_battPath = QString();
    double m_percentage = 0.0;
    bool m_isOnBatt = false;
};

#endif // UPOWERBACKEND_H
