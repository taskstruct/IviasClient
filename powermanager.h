#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <QObject>

#include "powerbackendiface.h"

class PowerManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal battery READ getBattery NOTIFY batteryValueChanged)

public:
    explicit PowerManager(QObject *parent = 0);
    ~PowerManager();

    void init();

    qreal getBattery() const { return m_batteryValue; }
    bool isOnBattery() const { return qobject_cast<PowerBackendIface*>( m_backend )->isOnBattery(); }

signals:
    // used in QML
    void batteryValueChanged();
    void batteryLow();
    void batteryCritical();
    void batteryBackToLow();
    void batteryBackToNormal();

    void powerSupplyPlugedIn();
    void powerSupplyPlugedOut();

private slots:
    void onBatteryValueChanged( double value );
    void onPowerSourceChanged( bool isOnBatt );

private:
    QObject *m_backend;
    double m_batteryValue;
};

#endif // POWERMANAGER_H
