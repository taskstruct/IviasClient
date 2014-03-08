#include "advertisementslist.h"

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QString>

#include <QtWidgets/QApplication>

#include <QtCore/QDebug>

#include "package.h"

//const QString cIviasClientPrivateFolder( QDir::homePath() + "/.IviasClient" );

AdvertisementsList *AdvertisementsList::self = 0;

AdvertisementsList::AdvertisementsList(QObject * parent):
    QObject(parent)
{
}

AdvertisementsList::~AdvertisementsList()
{
}

AdvertisementsList *AdvertisementsList::instance()
{
    if ( 0 == self )
    {
        self = new AdvertisementsList;
    }

    return self;
}

void AdvertisementsList::init()
{
    for( int i = 0; i < cTotalNumberOfAds; ++i )
    {
        m_ads[i] = new Package( i, this );
        m_ads[i]->load();
    }
}

//void AdvertisementsList::append(const AdType &ad)
//{
//    QList<AdType>::append(ad);

//    emit changed();
//}

const Package & AdvertisementsList::at ( int i ) const
{
    return *m_ads[i];
}

void AdvertisementsList::replace( int i, Package *newPgk )
{
    Q_ASSERT( i < cTotalNumberOfAds );

    Package *oldPgk = m_ads[i];
    oldPgk->remove();
    delete oldPgk;

    // set directory name to proper one
    newPgk->finish();

    m_ads[i] = newPgk;

    emit packageChanged(i);
}

void AdvertisementsList::clearList()
{
    for( int i = 0; i < cTotalNumberOfAds; ++i )
    {
        m_ads[i]->remove();
    }
}

