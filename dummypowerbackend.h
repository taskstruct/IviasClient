#ifndef DUMMYPOWERBACKEND_H
#define DUMMYPOWERBACKEND_H

#include <QtCore/QObject>

#include "powermanager.h"

class DummyPowerBackend : public QObject, public PowerBackendIface
{
    Q_OBJECT
    Q_INTERFACES(PowerBackendIface)
    
public:
    explicit DummyPowerBackend(QObject *parent = 0);
    ~DummyPowerBackend() {}

    bool isOnBattery() const { return m_step < 0.0; }

signals:
    void batteryValueChanged(double value);
    void powerSourceChanged( bool isOnBatt );

protected:
    void timerEvent(QTimerEvent *event);

private:
    double m_value = 100.0;
    double m_step = 1.0;

    int m_tid;
};

#endif // DUMMYPOWERBACKEND_H
