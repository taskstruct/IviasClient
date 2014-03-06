#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusPendingReply>

#include "upowerbackend.h"

#include <QDebug>

UPowerBackend::UPowerBackend(QObject *parent): PowerBackendIface(parent),
    m_dbusIface("org.freedesktop.UPower",
                "/org/freedesktop/UPower/devices/battery_BAT0",
                "org.freedesktop.UPower.Device",
                QDBusConnection::systemBus())
{
    m_percentage = getPercentage();

    qDebug() << "BATTERY: " << m_percentage;

    QDBusConnection::systemBus().connect( m_dbusIface.service(), m_dbusIface.path(), m_dbusIface.interface(), "Changed", this, SLOT(propertyChangedSlot()) );
}

void UPowerBackend::propertyChangedSlot()
{
    // some value changed. Check if it is percentage

    const double newValue = getPercentage();

    if( newValue != m_percentage ) {
        m_percentage = newValue;
        emit batteryChanged(m_percentage);
    }
}

double UPowerBackend::getPercentage() const
{
    double value = 0.0;
    QDBusMessage call = QDBusMessage::createMethodCall( m_dbusIface.service(), m_dbusIface.path(), "org.freedesktop.DBus.Properties", "Get");
    call << m_dbusIface.interface();
    call << "Percentage";

    QDBusPendingReply<QVariant> reply = QDBusConnection::systemBus().asyncCall(call);
    reply.waitForFinished();

    if (reply.isValid()) {
        value = reply.value().toDouble();
    }
    else {
        qDebug() << reply.error().name();
        qDebug() << reply.error().message();
    }

    return value;
}
