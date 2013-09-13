#ifndef CLICKSCOUNTER_H
#define CLICKSCOUNTER_H

#include <QObject>

class ClicksCounter : public QObject
{
    Q_OBJECT
public:
    explicit ClicksCounter(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // CLICKSCOUNTER_H
