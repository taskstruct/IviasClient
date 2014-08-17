#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QPluginLoader>
#include <QtCore/QString>

#include "powermanager.h"
#include "dummypowerbackend.h"

#include <QDebug>

static const double cCriticalLevel = 5.0;
static const double cLowLevel = 15.0;

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
    QDir pluginsDir( qApp->applicationDirPath() );

    m_backend = Q_NULLPTR;

    if( pluginsDir.cd("plugins") )
    {
        for( const QString pluginName: pluginsDir.entryList(QDir::Files) )
        {
            qDebug() << "PluginName: " << pluginName;

            QPluginLoader pluginLoader( pluginsDir.absoluteFilePath(pluginName) );

            const QJsonObject metaData = pluginLoader.metaData();

            const QJsonValue pluginType = metaData.value("type");

            qDebug() << "Metadata is undefined? " << pluginType.isUndefined();

//            if( !pluginType.isUndefined() && pluginType.isString() )
//            {
//                if( pluginType.toString().compare( "powerbackend", Qt::CaseInsensitive ) == 0 )
//                {
                    m_backend = pluginLoader.instance();

                    PowerBackendIface* backendIFace = qobject_cast< PowerBackendIface* >(m_backend);

                    if( Q_NULLPTR != backendIFace )
                    {
                        qDebug() << "Power manager plugin loaded!";
                        break;
                    }
                    else
                    {
                        // Backend does not implement PowerBackendIface
                        m_backend = Q_NULLPTR;
                    }
//                }
//            }
        }
    }

    if( Q_NULLPTR == m_backend )
    {
        qDebug() << "No backend found. Creating dummy one";
        m_backend = new DummyPowerBackend(this);
    }

    connect( m_backend, SIGNAL(batteryValueChanged(double)), this, SLOT(onBatteryValueChanged(double)) );
    connect( m_backend, SIGNAL(powerSourceChanged(bool)), this, SLOT(onPowerSourceChanged(bool)) );
}

void PowerManager::onBatteryValueChanged(double value)
{
    qDebug() << Q_FUNC_INFO << " value = " << value;

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

    emit batteryValueChanged();
}

void PowerManager::onPowerSourceChanged(bool isOnBatt)
{
    qDebug() << Q_FUNC_INFO << " isOnBatt = " << isOnBatt;

    if( isOnBatt ) {
        emit powerSupplyPlugedIn();
    }
    else {
        emit powerSupplyPlugedOut();
    }
}
