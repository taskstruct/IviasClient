#ifndef DUMMYPOWERBACKEND_H
#define DUMMYPOWERBACKEND_H

#include "powermanager.h"

class DummyPowerBackend : public PowerBackendIface
{
    Q_OBJECT
public:
    explicit DummyPowerBackend(QObject *parent = 0);
    ~DummyPowerBackend() {}

protected:
     void timerEvent(QTimerEvent *event);

private:
    int m_tid;
};

#endif // DUMMYPOWERBACKEND_H
