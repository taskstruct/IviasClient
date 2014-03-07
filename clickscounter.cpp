#include "clickscounter.h"
#include "advertisementslist.h"
#include "globals.h"
#include "helper.h"

#include <QtCore/QSettings>
#include <QtNetwork/QNetworkAccessManager>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <QtCore/QDebug>

//TODO: Move to globals
extern QNetworkAccessManager *gNetworkAccessManager;
extern const QLatin1String IviasClientDBConnection;

ClicksCounter::ClicksCounter(QObject *parent) :
    QObject(parent),
    m_clicksQueue()
{
}

ClicksCounter::~ClicksCounter()
{
    save();
}

void ClicksCounter::increment( int adUid )
{
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );

    qDebug() << "ClicksCounter::increment() " << adUid;

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible && db.isOpen() )
    {
        // update DB
        const QLatin1Literal command("UPDATE Stats SET clicks = clicks+1 WHERE icId = %1 AND adUID = %2");
        QSqlQuery query( QString(command).arg(gIviasClientID).arg(adUid), db );

        if(!query.exec()) {
            updateItem(adUid);
        }
    }
    else
    {
        updateItem(adUid);
    }
}

void ClicksCounter::init()
{
    QSettings settings(Helper::getSettingsPath(), QSettings::IniFormat, this);


    int arraySize = settings.beginReadArray("clicksQueue");

    for( int i = 0; i < arraySize; ++i )
    {
        settings.setArrayIndex(i);
        const int adUID = settings.value("aduid").toInt();
        const int clicks = settings.value("clicks").toInt();
        m_clicksQueue.insert( adUID, clicks );
    }

    settings.endArray();
}

void ClicksCounter::flushQueue()
{
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible && db.isOpen() )
    {
        const QLatin1Literal command("UPDATE stats SET clicks = clicks+%1 WHERE cid = %2 AND adUID = %3");

        QMapIterator<int, int> iter(m_clicksQueue);
        while (iter.hasNext()) {
            iter.next();

            if( iter.value() != 0 ) {
                QSqlQuery query( QString(command).arg(iter.value()).arg(gIviasClientID).arg(iter.key()), db );

                if( query.exec() ) {
                    m_clicksQueue[iter.key()] = 0;
                }
            }
        }
    }

    // remove all send values
    save();
}


void ClicksCounter::updateItem(int uid)
{
    if( m_clicksQueue.contains(uid) ) {
        m_clicksQueue[uid]++;
    }
    else {
        m_clicksQueue.insert( uid, 1 );
    }
}

void ClicksCounter::save()
{
    qDebug() << "Saving clicks queue";

    QSettings settings(Helper::getSettingsPath(), QSettings::IniFormat, this);

    settings.clear();

    settings.beginWriteArray("clicksQueue");

    QMapIterator<int, int> iter(m_clicksQueue);
    int i = 0;
    while (iter.hasNext()) {
        iter.next();

        settings.setArrayIndex(i);

        // save only not send values
        if( iter.value() != 0 ) {
            settings.setValue("aduid", iter.key());
            settings.setValue("clicks", iter.value());
            ++i;
        }
    }

    settings.endArray();
}
