#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingReply>

#include "upowerbackend.h"

#include <QDebug>

const QString cUPowerService( QStringLiteral("org.freedesktop.UPower") );
const QString cUPowerInterface( QStringLiteral("org.freedesktop.UPower") );
const QString cUPowerPath( QStringLiteral("/org/freedesktop/UPower") );
const QString cDeviceInterface( QStringLiteral("org.freedesktop.UPower.Device") );
const QString cDBusPropertiesInterface( QStringLiteral("org.freedesktop.DBus.Properties") );

UPowerBackend::UPowerBackend(QObject *parent): PowerBackendIface(parent)
{
    QDBusMessage call = QDBusMessage::createMethodCall( cUPowerService, cUPowerPath, cUPowerInterface, "EnumerateDevices" );
    QDBusPendingCall pendingCall = QDBusConnection::systemBus().asyncCall(call);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher( pendingCall, this );

    connect( watcher, &QDBusPendingCallWatcher::finished, this, &UPowerBackend::onEnumerateDevicesFinished );
}

void UPowerBackend::onEnumerateDevicesFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply< QList<QDBusObjectPath> > reply = *watcher;

    if( !reply.isError() )
    {
        for( auto &e: reply.value() )
        {
            const QString &p = e.path();

            qDebug() << e.path();

            if( p.contains( QStringLiteral("battery_BAT0") ) )
            {
                m_battPath = p;
                setupBattery();
                break;
            }
        }
    }
    else
    {
        qDebug() << reply.error().name();
        qDebug() << reply.error().message();
    }

    watcher->deleteLater();
}

void UPowerBackend::onPropertyChanged()
{
    // some value changed. Check if it is percentage
    updatePercentage();
}

void UPowerBackend::setupBattery()
{
    updatePercentage();
    QDBusConnection::systemBus().connect( cUPowerService, m_battPath, cDeviceInterface, "Changed", this, SLOT(onPropertyChanged()) );
}

void UPowerBackend::updatePercentage()
{
    QDBusMessage call = QDBusMessage::createMethodCall( cUPowerService, m_battPath, cDBusPropertiesInterface, "Get" );
    call << cDeviceInterface;
    call << "Percentage";
    QDBusPendingCall pendingCall = QDBusConnection::systemBus().asyncCall(call);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher( pendingCall, this );

    connect( watcher, &QDBusPendingCallWatcher::finished, this, &UPowerBackend::onPercentageFinished );
}

void UPowerBackend::onPercentageFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply< QVariant > reply = *watcher;

    if( !reply.isError() )
    {
        const double newValue = reply.value().toDouble();
        if( newValue != m_percentage ) {
            m_percentage = newValue;
            qDebug() << "New battery value: " << m_percentage;
            emit batteryChanged(m_percentage);
        }
    }
    else
    {
        qDebug() << reply.error().name();
        qDebug() << reply.error().message();
    }

    watcher->deleteLater();
}
