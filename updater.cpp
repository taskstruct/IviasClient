#include "updater.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include <QtCore/QTimerEvent>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <QtCore/QDebug>

#include "advertisementslist.h"
#include "globals.h"
#include "helper.h"
#include "powermanager.h"
#include "qmlsettingssingleton.h"

Updater::Updater(QObject *parent) :
    QObject(parent),
    m_state( Sleeping ),
    m_currentAdIndex(-1),
    m_timerId(0)
{
}

void Updater::update()
{
    if( QMLSettingsSingleton::instance()->url().isValid() )
    {
        requestNext();

        qDebug() << "Starting update ...";
    }
}

void Updater::finishUpdate()
{
    // update times has expired
    m_currentAdIndex = -1;

    // restart timer
    m_timerId = startTimer( QMLSettingsSingleton::instance()->updateInterval() );
    m_state = Sleeping;
}

void Updater::timerEvent ( QTimerEvent * event )
{
    if( event->timerId() == m_timerId )
    {
        killTimer(m_timerId);
        m_timerId = 0;

        if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible )
        {
            update();
        }
    }
}

void Updater::packageUnpacked(bool ok)
{
    qDebug() << "Package unpacked " << ok;

    // disconnect package signal. It is not needed anymore
    disconnect( m_pkg, &Package::packageReady, this, &Updater::packageUnpacked );

    if( false != ok )
    {
        // update model
        AdvertisementsList::instance()->replace( m_currentAdIndex, m_pkg );
    }
    else
    {
        m_pkg->deleteLater();
    }

    requestNext();
}

void Updater::requestFile()
{
    qDebug() << "Reguesting file " << m_currentAdUrl;

    QNetworkRequest request( m_currentAdUrl );
    QNetworkReply *reply = gNetworkAccessManager->get( request );

    if( m_pkg->openTmpFile() )
    {
        connect( reply, &QIODevice::readyRead, m_pkg, &Package::newDataAvailable );
        connect( reply, &QNetworkReply::finished, this, &Updater::downloadFinished );

        m_state = Downloading;
    }
    else
    {
        delete m_pkg;
        m_pkg = nullptr;

        requestNext();
    }
}

void Updater::downloadFinished()
{
    QNetworkReply * reply = qobject_cast<QNetworkReply *>( sender() );

    Q_ASSERT( 0 == reply );

    qDebug() << " File downloaded " << reply->url() << "\nStatus: " << reply->error();

    if( Downloading == m_state )
    {
        if( QNetworkReply::NoError == reply->error() )
        {
            connect( m_pkg, &Package::packageReady, this, &Updater::packageUnpacked );

            m_pkg->unpack();

            m_state = Unpacking;
        }
        else
        {
            delete m_pkg;
            m_pkg = nullptr;
            // skip to next one
            qDebug() << "File downloaded with error " << reply->errorString();
            requestNext();
        }
    }
    else
    {
        delete m_pkg;
        m_pkg = nullptr;
        // this should not happened
        Q_ASSERT(false);
    }

    // schedule delete of replay. We don't need it anymore
    reply->deleteLater();
}

void Updater::constructAdUrl()
{
    QUrl url = QMLSettingsSingleton::instance()->url();
    m_currentAdUrl.setHost( url.host() );
    m_currentAdUrl.setPath( url.path() + "/Ad" + QString::number( m_currentAdIndex ) + ".7z");
    m_currentAdUrl.setPort( QMLSettingsSingleton::instance()->port() );
    m_currentAdUrl.setScheme( QLatin1String("http") );
}

void Updater::requestLastModfied()
{
    QNetworkRequest request( m_currentAdUrl );

    QNetworkReply * reply = gNetworkAccessManager->head( request );
    connect( reply, &QNetworkReply::finished, this, &Updater::headersReceived );
    m_state = GettingHeaders;
}

void Updater::headersReceived()
{
    QNetworkReply * reply = qobject_cast<QNetworkReply *>( sender() );

    Q_ASSERT( 0 != reply );

    qDebug() << " Headers received " << reply->url() << "\nStatus: " << reply->error();

    if( GettingHeaders == m_state )
    {
        if( QNetworkReply::NoError == reply->error() )
        {
            QDateTime lastModified = reply->header( QNetworkRequest::LastModifiedHeader ).toDateTime();

            if( AdvertisementsList::instance()->at( m_currentAdIndex ).lastModified() == lastModified )
            {
                // ad didn't changed. Go to next one if any
                requestNext();
            }
            else
            {
                m_pkg = new Package( m_currentAdIndex );
                m_pkg->setLastModified( lastModified );

                requestFile();
            }
        }
        else
        {
            qDebug() << "Error occured during headers request" << reply->errorString();
            // try to get next one
            requestNext();
        }
    }
    else
    {
        // this should not happen
        Q_ASSERT( false );
    }

    // schedule delete of replay. We don't need it anymore
    reply->deleteLater();
}

void Updater::requestNext()
{
    if( !canMakeUpdate() )
    {
        return;
    }

    ++m_currentAdIndex;

    if( m_currentAdIndex < cTotalNumberOfAds )
    {
        constructAdUrl();
        requestLastModfied();
    }
    else
    {
        finishUpdate();
    }
}

bool Updater::canMakeUpdate() const
{
    if( 0 != m_timerId )
    {
        // update timer isn't finished
        return false;
    }

    if( gNetworkAccessManager->networkAccessible() != QNetworkAccessManager::Accessible )
    {
        // no network
        return false;
    }

    if( gPowerManager->isOnBattery() )
    {
        // update only when AC power is on
        return false;
    }

    return true;
}
