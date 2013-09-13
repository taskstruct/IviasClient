#include "clickscounter.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

extern QNetworkAccessManager *gNetworkAccessManager;

ClicksCounter::ClicksCounter(QObject *parent) :
    QObject(parent)
{
    qMemSet( m_clicksQueue, 0, sizeof(m_clicksQueue) );
}

void ClicksCounter::update( int page, int index )
{
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );
    const int id = page * cAdsPerPage + index;

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible && db.isOpen() )
    {
        // update DB
        const QString1Literal command("UPDATE stats SET clicks = clicks+1 WHERE cid = %1 AND adId = %2");
        QSqlQuery query( QString(command).arg(/*TODO: client id*/).arg(id), db );

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
        const QString1Literal command("UPDATE stats SET clicks = clicks+%1 WHERE cid = %2 AND adId = %3");

        for( int i = 0; i < cTotalNumberOfAds; i++ )
        {
            if( m_clicksQueue[i] != 0 ) {
                QSqlQuery query( QString(command).arg(m_clicksQueue[i]).arg(/*TODO: client id*/).arg(id), db );

                if( query.exec() ) {
                    m_clicksQueue[i] = 0;
                }
            }
        }
    }
}
