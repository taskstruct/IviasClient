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

Updater::Updater(QObject *parent) :
    QObject(parent),
    m_state( Sleeping ),
    m_currentAdIndex(-1),
    m_timerId(0)
{
}

void Updater::update()
{
    if( 0 == m_timerId )
    {
        // update times has expired
        m_currentAdIndex = -1;

        requestNext();

        qDebug() << "Starting update ...";
    }
}

void Updater::finishUpdate()
{
    // restart timer
    m_timerId = startTimer( gQmlSettings->updateInterval() );
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
    disconnect( m_pkg, SIGNAL(packageReady(bool)), this, SLOT(packageUnpacked(bool)) );

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

    connect( reply, SIGNAL(finished()), this, SLOT(downloadFinished()) );

    m_state = Downloading;
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
            // save package to /tmp
            QFile tmp( Helper::tempPackageName() );

            if( tmp.open( QIODevice::WriteOnly ) )
            {
                quint64 rBufSize = reply->readBufferSize();
                quint64 wBufSize = tmp.write( reply->readAll() );
                tmp.close();

                if( rBufSize == wBufSize )
                {
                    m_pkg = new Package( m_currentAdIndex );
                    m_pkg->setLastModified( reply->header( QNetworkRequest::LastModifiedHeader ).toDateTime() );

                    connect( m_pkg, SIGNAL(packageReady(bool)), this, SLOT(packageUnpacked(bool)) );

                    m_pkg->unpack();

                    m_state = Unpacking;
                }
                else
                {
                    // file is broken. Go to next one
                    requestNext();
                }
            }
            else
            {
                Q_ASSERT( false );
            }
        }
        else
        {
            // skip to next one
            qDebug() << "File downloaded with error " << reply->errorString();
            requestNext();
        }
    }
    else
    {
        // this should not happened
        Q_ASSERT(false);
    }

    // schedule delete of replay. We don't need it anymore
    reply->deleteLater();
}

void Updater::constructAdUrl()
{
    QUrl url = gQmlSettings->url();
    m_currentAdUrl.setHost( url.host() );
    m_currentAdUrl.setPath( url.path() + "/Ad" + QString::number( m_currentAdIndex ) + ".7z");
    m_currentAdUrl.setPort( gQmlSettings->port() );
    m_currentAdUrl.setScheme( QLatin1String("http") );
}

void Updater::requestLastModfied()
{
    QNetworkRequest request( m_currentAdUrl );

    QNetworkReply * reply = ConfigurationManager::instance()->networkAccessManager()->head( request );
    connect( reply, SIGNAL(finished()), this, SLOT(headersReceived()) );
    m_state = GettingHeaders;
}

void Updater::headersReceived()
{
    QNetworkReply * reply = qobject_cast<QNetworkReply *>( sender() );

    Q_ASSERT( 0 == reply );

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
