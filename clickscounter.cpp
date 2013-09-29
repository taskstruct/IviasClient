#include "clickscounter.h"
#include "globals.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QtCore/QDebug>

extern QNetworkAccessManager *gNetworkAccessManager;
extern const QLatin1String IviasClientDBConnection;

ClicksCounter::ClicksCounter(QObject *parent) :
    QObject(parent)
{
    memset( m_clicksQueue, 0, sizeof(m_clicksQueue) );
}

void ClicksCounter::increment( int page, int index )
{
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );
    const int id = page * cAdsPerPage + index;

    qDebug() << "ClicksCounter::increment( int page, int index ) " << page << " " << index;

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible && db.isOpen() )
    {
        // update DB
        const QLatin1Literal command("UPDATE stats SET clicks = clicks+1 WHERE cid = %1 AND adId = %2");
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
