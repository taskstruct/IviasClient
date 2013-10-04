#include "clickscounter.h"
#include "globals.h"
#include "helper.h"

#include <QtCore/QSettings>
#include <QtNetwork/QNetworkAccessManager>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QtCore/QDebug>

extern QNetworkAccessManager *gNetworkAccessManager;
extern const QLatin1String IviasClientDBConnection;

ClicksCounter::ClicksCounter(QObject *parent) :
    QObject(parent)
{
    m_clicksQueue.reserve(cTotalNumberOfAds);

    QSettings settings(Helper::getSettingsPath(), QSettings::IniFormat, this);

    m_clicksQueue = settings.value("clicksQueue", QList::fromStdList( std::list<int>(cTotalNumberOfAds), 0 ) ).toList<int>();

    Q_ASSERT( m_clicksQueue.size() == cTotalNumberOfAds );
}

ClicksCounter::~ClicksCounter()
{
    qDebug() << "Saving clicks queue";
    QSettings settings(Helper::getSettingsPath(), QSettings::IniFormat, this);
    settings.setValue("clicksQueue", m_clicksQueue);
}

void ClicksCounter::increment( int page, int index )
{
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );
    const int id = page * cAdsPerPage + index;

    qDebug() << "ClicksCounter::increment( int page, int index ) " << page << " " << index;

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible && db.isOpen() )
    {
        // update DB
        const QLatin1Literal command("UPDATE Stats SET clicks = clicks+1 WHERE icId = %1 AND adIndex = %2");
        QSqlQuery query( QString(command).arg(gIviasClientID).arg(id), db );

        if(!query.exec()) {
            // error. Add click to queue
            m_clicksQueue[id]++;
        }
    }
    else
    {
        // update queue
        m_clicksQueue[id]++;
    }
}

void ClicksCounter::flushQueue()
{
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible && db.isOpen() )
    {
        const QLatin1Literal command("UPDATE stats SET clicks = clicks+%1 WHERE cid = %2 AND adId = %3");

        Q_ASSERT( m_clicksQueue.size() == cTotalNumberOfAds );
        for( int i = 0; i < cTotalNumberOfAds; i++ )
        {
            if( m_clicksQueue[i] != 0 ) {
                QSqlQuery query( QString(command).arg(m_clicksQueue[i]).arg(gIviasClientID).arg(i), db );

                if( query.exec() ) {
                    m_clicksQueue[i] = 0;
                }
            }
        }
    }
}
