#include <QtCore/QTimerEvent>

#include "dummypowerbackend.h"

DummyPowerBackend::DummyPowerBackend(QObject *parent) :
    PowerBackendIface(parent)
{
    // 10 mins
    m_tid = startTimer( 10 * 60 * 1000 );
}

void DummyPowerBackend::timerEvent(QTimerEvent *event)
{
    static double value = 100.0;
    static double step = 1.0;

    if( event->timerId() == m_tid ) {
        if( qFuzzyCompare( value, 100.0 ) ) {
            step = -step;
        }

        if( qFuzzyCompare( value, 0.0 ) ) {
            step = -step;
        }

        value += step;
        emit batteryChanged(value);
    }
}
