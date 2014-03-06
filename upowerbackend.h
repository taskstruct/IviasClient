#ifndef UPOWERBACKEND_H
#define UPOWERBACKEND_H

#include <QtDBus/QDBusInterface>

#include "powermanager.h"

class UPowerBackend: public PowerBackendIface
{
    Q_OBJECT

public:
    UPowerBackend( QObject *parent = 0 );
    ~UPowerBackend() {}

private slots:
    void propertyChangedSlot();

private:
    double getPercentage() const;

    QDBusInterface m_dbusIface;
    double m_percentage;
};

#endif // UPOWERBACKEND_H
