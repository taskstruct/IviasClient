#include <QtCore/QTimerEvent>

#include "dummypowerbackend.h"

#include <QDebug>

DummyPowerBackend::DummyPowerBackend(QObject *parent) :
    QObject(parent),
    PowerBackendIface()
{
    // 10 mins
    m_tid = startTimer( 1 * 1 * 1000 );
}

void DummyPowerBackend::timerEvent(QTimerEvent *event)
{
    qDebug() << "Timer: " << m_value << " " << m_step;

    if( event->timerId() == m_tid ) {
        if( qFuzzyCompare( m_value, 100.0 ) ) {
            m_step = -m_step;

            emit powerSourceChanged( false );
        }

        if( qFuzzyCompare( m_value, 0.0 ) ) {
            m_step = -m_step;

            emit powerSourceChanged( true );
        }

        m_value += m_step;
        emit batteryValueChanged(m_value);
    }
}
