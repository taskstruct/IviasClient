#include <QtCore/QTimerEvent>

#include "dummypowerbackend.h"

DummyPowerBackend::DummyPowerBackend(QObject *parent) :
    QObject(parent),
    PowerBackendIface()
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

            emit powerSourceChanged( true );
        }

        if( qFuzzyCompare( value, 0.0 ) ) {
            step = -step;

            emit powerSourceChanged( false );
        }

        value += step;
        emit batteryValueChanged(value);
    }
}
