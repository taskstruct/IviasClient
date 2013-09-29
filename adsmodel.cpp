#include "adsmodel.h"

#include <QtCore/QHash>

#include "advertisementslist.h"

AdsModel::AdsModel(QObject *parent) :
    QAbstractListModel(parent),
    m_page(0)
{
    connect( AdvertisementsList::instance(), SIGNAL(packageChanged(int)), this, SLOT(packageChanged(int)) );
}

AdsModel::~AdsModel()
{
}

int AdsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return cAdsPerPage;
}

QVariant AdsModel::data(const QModelIndex &index, int role) const
{
    QVariant retVal;

    if( index.isValid() )
    {
        int row = index.row();

        if( row < cAdsPerPage )
        {
            if( TitleRole == role )
            {
                retVal = AdvertisementsList::instance()->at( m_page * cAdsPerPage + row ).title();
            }
            else if( ThumbnailPathRole == role )
            {
                retVal = AdvertisementsList::instance()->at( m_page * cAdsPerPage + row ).thumbnail();
            }
            else if( FlashPathRole == role )
            {
                retVal = AdvertisementsList::instance()->at( m_page * cAdsPerPage + row ).startFile();
            }
        }
        else
        {
            Q_ASSERT( false );
        }
    }
    else
    {
        Q_ASSERT( index.isValid() );
    }

    return retVal;
}

int AdsModel::page() const
{
    return m_page;
}

void AdsModel::setPage(int page)
{
    if( m_page != page ) {
        m_page = page;

        emit pageChanged();
        emit dataChanged( index( 0, 0 ), index( cAdsPerPage - 1, 0 ) );
    }
}

QString AdsModel::getLink(int i)
{
    return AdvertisementsList::instance()->at( i ).startFile();
}

int AdsModel::count() const
{
    return cAdsPerPage;
}

QHash<int,QByteArray> AdsModel::roleNames() const
{
    QHash< int, QByteArray > roles;
    roles[ TitleRole ] = "title";
    roles[ ThumbnailPathRole ] = "thumbnail";
    roles[ FlashPathRole ] = "startpage";

    return roles;
}

void AdsModel::classBegin()
{
}

void AdsModel::componentComplete()
{
//    if (!d->folder.isValid() || d->folder.toLocalFile().isEmpty() || !QDir().exists(d->folder.toLocalFile()))
//        setFolder(QUrl(QLatin1String("file://")+QDir::currentPath()));

//    if (!d->folderIndex.isValid())
//        QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection);
}

void AdsModel::packageChanged(int i )
{
    if( ( i / cAdsPerPage ) == m_page )
    {
        // new package is on current page
        int row = i % cAdsPerPage;
        emit dataChanged( index( row, 0 ), index( row, 0 ) );
    }
}
