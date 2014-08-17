#ifndef UPOWERBACKEND_H
#define UPOWERBACKEND_H

#include <QtCore/QtPlugin>
#include <QtDBus/QDBusInterface>

#include "upowerbackend_export.h"
#include "../powerbackendiface.h"

class QDBusPendingCallWatcher;

class UPOWERBACKEND_EXPORT UPowerBackend: public QObject, public PowerBackendIface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PowerBackendInterface_iid FILE "powerbackends.json")
    Q_INTERFACES(PowerBackendIface)

public:
    UPowerBackend( QObject *parent = 0 );
    ~UPowerBackend() {}

    bool isOnBattery() const { return m_isOnBatt; }

signals:
    void batteryValueChanged(double newValue);
    void powerSourceChanged( bool isOnBatt );

private slots:
    void onEnumerateDevicesFinished(QDBusPendingCallWatcher *watcher);
    void onBatteryPropertyChanged();
    void onUPowerPropertyChanged();

private:
    QDBusInterface *m_uPowerIFace;
    QDBusInterface *m_battDevIFace = Q_NULLPTR;

    QString m_battPath = QString();
    double m_percentage = 0.0;
    bool m_isOnBatt = false;
};

#endif // UPOWERBACKEND_H
