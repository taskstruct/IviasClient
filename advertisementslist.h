#ifndef ADVERTISEMENTSLIST_H
#define ADVERTISEMENTSLIST_H

#include <QtCore/QObject>

#include "constraints.h"
#include "package.h"

class AdvertisementsList: public QObject
{
    Q_OBJECT

    AdvertisementsList(QObject * parent = 0);

public:
    ~AdvertisementsList();

    static AdvertisementsList *instance();

    void init();

//    void append(const AdType &ad);
    const Package & at ( int i ) const;
    void replace( int i, Package *newPgk );
    void clearList();

signals:
    void packageChanged(int);

private:
    static AdvertisementsList *self;

    Package* m_ads[ cTotalNumberOfAds ];
};

#endif // ADVERTISEMENTSLIST_H
