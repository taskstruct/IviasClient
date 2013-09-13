#ifndef CLICKSCOUNTER_H
#define CLICKSCOUNTER_H

#include <QtCore/QObject>
#include <QtCore/QPair>

#include "constraints.h"

class ClicksCounter : public QObject
{
    Q_OBJECT
public:
    explicit ClicksCounter(QObject *parent = 0);

    Q_INVOKABLE void update( int page, int index );

    void flushQueue();
    
    //TODO: Save queue on exit, if there are unupdated values

private:
    int m_clicksQueue[cTotalNumberOfAds];
};

#endif // CLICKSCOUNTER_H
