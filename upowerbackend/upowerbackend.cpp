#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingReply>

#include "upowerbackend.h"

#include <QDebug>

const QString cUPowerService( QStringLiteral("org.freedesktop.UPower") );
const QString cUPowerInterface( QStringLiteral("org.freedesktop.UPower") );
const QString cUPowerPath( QStringLiteral("/org/freedesktop/UPower") );
const QString cDeviceInterface( QStringLiteral("org.freedesktop.UPower.Device") );
const QString cDBusPropertiesInterface( QStringLiteral("org.freedesktop.DBus.Properties") );

const char* cOnBattProp = "OnBattery";
const char* cPercentageProp = "Percentage";

#include <QMetaMethod>
#include <QMetaProperty>
#include <cstring>

UPowerBackend::UPowerBackend(QObject *parent): QObject(parent), PowerBackendIface(),
    m_uPowerIFace( new QDBusInterface( cUPowerService, cUPowerPath, cUPowerInterface, QDBusConnection::systemBus(), this ) )
{
    m_isOnBatt = m_uPowerIFace->property( cOnBattProp ).toBool();

    QDBusConnection::systemBus().connect( cUPowerService, cUPowerPath, cUPowerInterface, "Changed", this, SLOT(onUPowerPropertyChanged()) );

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

            qDebug() << p;

            if( p.contains( QStringLiteral("battery_BAT0") ) )
            {
                m_battPath = p;

                QDBusConnection sysBus( QDBusConnection::systemBus() );

                sysBus.connect( cUPowerService, m_battPath, cDeviceInterface, "Changed", this, SLOT(onBatteryPropertyChanged()) );

                m_battDevIFace = new QDBusInterface( cUPowerService, m_battPath, cDeviceInterface, sysBus, this );
                m_percentage = m_battDevIFace->property( cPercentageProp ).toDouble();

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

void UPowerBackend::onUPowerPropertyChanged()
{
    const bool newVal = m_uPowerIFace->property( cOnBattProp ).toBool();

    if( newVal != m_isOnBatt ) {
        m_isOnBatt = newVal;
        emit powerSourceChanged( m_isOnBatt );
    }
}

void UPowerBackend::onBatteryPropertyChanged()
{
    // some value changed. Check if it is percentage

    const double newVal = m_battDevIFace->property( cPercentageProp ).toDouble();

    if( newVal != m_percentage ) {
        m_percentage = newVal;
        emit batteryValueChanged(m_percentage);
    }
}
