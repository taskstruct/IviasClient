#ifndef INPUTACTIVITYFILTER_H
#define INPUTACTIVITYFILTER_H

#include <QObject>

class InputActivityFilter : public QObject
{
    Q_OBJECT
public:
    explicit InputActivityFilter(QObject *parent = 0);
    
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void timerEvent(QTimerEvent *event );

private:
    int m_timerId;
};

#endif // INPUTACTIVITYFILTER_H
