#include <QtCore/QEvent>

#include "inputactivityfilter.h"

#include <QtCore/QDebug>

InputActivityFilter::InputActivityFilter(QObject *parent) :
    QObject(parent),
    m_timerId(0)
{
    m_timerId = startTimer(10*60*1000);
}

bool InputActivityFilter::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type t = event->type();

    if ( ( QEvent::KeyPress == t ) ||
         ( QEvent::MouseButtonPress == t ) ||
         ( QEvent::TouchBegin == t ) )
    {
        qDebug() << "Filter event" << t;
        // restart timer
        killTimer(m_timerId);
        m_timerId = startTimer(10*60*1000);
    }

    // standard event processing
    return false;
}

void InputActivityFilter::timerEvent(QTimerEvent *event )
{
    if( event->timerId() == m_timerId ) {
        parent()->setProperty("screenLocked", true);
        killTimer(m_timerId);
        m_timerId = 0;
    }
}
