#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <QObject>

class PowerBackendIface: public QObject
{
    Q_OBJECT
public:
    PowerBackendIface(QObject *parent = 0);
    virtual ~PowerBackendIface() {}

    virtual bool isOnBattery() const = 0;
signals:
    void batteryChanged(double newValue);

private:
};

class PowerManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal battery READ getBattery NOTIFY batteryChanged)

public:
    explicit PowerManager(QObject *parent = 0);
    ~PowerManager();

    void init();

    qreal getBattery() const { return m_batteryValue; }
    bool isOnBattery() const { return m_backend->isOnBattery(); }

signals:
    // used in QML
    void batteryChanged();
    void batteryLow();
    void batteryCritical();
    void batteryBackToLow();
    void batteryBackToNormal();

    void powerSupplyPlugedIn();
    void powerSupplyPlugedOut();

private slots:
    void onBatteryChanged( double value );

private:
    PowerBackendIface *m_backend;
    double m_batteryValue;
};

#endif // POWERMANAGER_H
