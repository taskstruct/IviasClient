#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <QObject>

class PowerBackendIface: public QObject
{
    Q_OBJECT
public:
    PowerBackendIface(QObject *parent = 0);
    virtual ~PowerBackendIface() {}

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

signals:
    // used in QML
    void batteryChanged();
    void batteryLow();
    void batteryCritical();
    void batteryBackToLow();
    void batteryBackToNormal();

private slots:
    void onBatteryChanged( double value );

private:
    PowerBackendIface *m_backend;
    double m_batteryValue;
};

#endif // POWERMANAGER_H
