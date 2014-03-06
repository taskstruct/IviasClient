#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "powermanager.h"

#include "upowerbackend.h"
#include "dummypowerbackend.h"

#include <QDebug>

static const double cCriticalLevel = 5.0;
static const double cLowLevel = 15.0;

static bool isUbuntu()
{
    QFile os_release("/etc/os-release");

    if( !os_release.exists() ) {
        return false;
    }

    if( !os_release.open( QIODevice::ReadOnly ) ) {
        return false;
    }

    QTextStream stream(&os_release);
    QString line;

    do {
      line = stream.readLine();

      if( line.startsWith( QLatin1String("ID="), Qt::CaseInsensitive ) ) {
          QStringList lineSplit = line.split( QChar('=') );

          if( lineSplit.at(1).compare( QLatin1String("ubuntu"), Qt::CaseInsensitive ) == 0 ) {
              return true;
          }
      }

    }
    while( !line.isNull() );

    return false;
}

PowerBackendIface::PowerBackendIface(QObject *parent):
    QObject(parent)
{
}



PowerManager::PowerManager(QObject *parent) :
    QObject(parent),
    m_backend(Q_NULLPTR),
    m_batteryValue(0.0)
{
}

PowerManager::~PowerManager()
{
    if(m_backend) {
        delete m_backend;
    }
}

void PowerManager::init()
{
    if( isUbuntu() ) {
        qDebug() << "Creating Ubuntu backend";
        m_backend = new UPowerBackend(this);
    }
    else {
        qDebug() << "Creating dummy backend";
        m_backend = new DummyPowerBackend(this);
    }

    connect( m_backend, &PowerBackendIface::batteryChanged, this, &PowerManager::onBatteryChanged );
}

void PowerManager::onBatteryChanged(double value)
{
    const double oldValue = m_batteryValue;

    m_batteryValue = value;

    if( m_batteryValue < cCriticalLevel ) {
        if( oldValue >= cCriticalLevel ) {
            emit batteryCritical();
        }
    }
    else if( m_batteryValue < cLowLevel ) {
        if( oldValue >= cLowLevel ) {
            emit batteryLow();
        }
        else if( oldValue < cCriticalLevel ) {
            emit batteryBackToLow();
        }
    }
    else if( m_batteryValue >= cLowLevel ) {
        if( oldValue < cLowLevel ) {
            emit batteryBackToNormal();
        }
    }
    emit batteryChanged();
}
