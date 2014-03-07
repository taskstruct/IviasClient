#ifndef CLICKSCOUNTER_H
#define CLICKSCOUNTER_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "constraints.h"

class ClicksCounter : public QObject
{
    Q_OBJECT
public:
    explicit ClicksCounter(QObject *parent = 0);
    ~ClicksCounter();

    Q_INVOKABLE void increment(int adUid);

    void init();
    void flushQueue();
    
private:
    void updateItem(int uid);
    void save();

    QMap<int, int> m_clicksQueue;
};

#endif // CLICKSCOUNTER_H
