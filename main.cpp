#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtGui/QGuiApplication>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkReply>
#include <QtQml/qqml.h>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlNetworkAccessManagerFactory>
#include <QtQuick/QQuickWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include <iostream>

#include "adsmodel.h"
#include "advertisementslist.h"
#include "clickscounter.h"
#include "globals.h"
#include "helper.h"
#include "inputactivityfilter.h"
#include "powermanager.h"
#include "qmlsettingssingleton.h"
#include "updater.h"

const QLatin1String IviasClientDBConnection("icdbc");

QNetworkAccessManager *gNetworkAccessManager;
ClicksCounter *gClicksCounter = Q_NULLPTR;
PowerManager *gPowerManager = Q_NULLPTR;
int gIviasClientID = 0;

class IviasQmlNetworkAccessManagerFactory: public QQmlNetworkAccessManagerFactory {

    virtual QNetworkAccessManager *	create(QObject * parent)
    {
        Q_UNUSED(parent)

        return gNetworkAccessManager;
    }
};


static QObject *qml_settings_singleton_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return QMLSettingsSingleton::instance();
}

static QObject *clicks_counter_singleton_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    if( !gClicksCounter ) {
        gClicksCounter = new ClicksCounter();
    }

    return gClicksCounter;
}

static QObject *power_manager_singleton_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return gPowerManager;
}

void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator * authenticator)
{
    if( QMLSettingsSingleton::instance()->url().isParentOf(reply->request().url()) ) {
        authenticator->setUser(QMLSettingsSingleton::instance()->username());
        authenticator->setPassword(QMLSettingsSingleton::instance()->password());
    }
    else {
        qDebug() << "Trying to authenticate with wrong url: " << reply->request().url() <<
                    "\nProject url is: " << QMLSettingsSingleton::instance()->url();
    }
}

void onNetworkAccessibleChanged ( QNetworkAccessManager::NetworkAccessibility accessible )
{
    if( QNetworkAccessManager::Accessible == accessible )
    {
        QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );

        Q_ASSERT( !db.isValid() );

        if( !db.isOpen() ) {
            if(db.open()) {
                QMLSettingsSingleton::instance()->refetchData();
            }
        }

        //TODO: Something with Updater???
    }
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName( "Ivias" );
    app.setOrganizationDomain( "ivias.org" );
    app.setApplicationName( "IviasClient" );
    // GUI
    qmlRegisterType<AdsModel>( "IviasClient", 1, 0, "AdsModel" );;

    qmlRegisterSingletonType<QMLSettingsSingleton>("IviasClient", 1, 0, "IviasSettings", qml_settings_singleton_provider);
    qmlRegisterSingletonType<ClicksCounter>("IviasClient", 1, 0, "ClickCounter", clicks_counter_singleton_provider);
    qmlRegisterSingletonType<PowerManager>("IviasClient", 1, 0, "PowerManager", power_manager_singleton_provider);

    // init network manager
    gNetworkAccessManager = new QNetworkAccessManager(&app);

    // Read systemSettings.ini
    QSettings systemSettings( Helper::getSystemSettingsPath(), QSettings::IniFormat, &app );
    gIviasClientID = systemSettings.value("iviasClientID").toInt();

    // init DB
    std::cout << "Setup SQL access..." << std::endl;
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", IviasClientDBConnection );

    if(!db.isValid()) {
        // unable to create a valid DB connection. Stop application
        qDebug("Unable to create a valid DB connection");
        return -1; //TODO: Show this as error message in app, because if app exits, plain  X server will be shown
    }

    db.setHostName(systemSettings.value("dbHostName").toString());
//    db.setPort(systemSettings.value("dbPort", 3306));
    db.setUserName(systemSettings.value("dbUserName").toString());
    db.setPassword(systemSettings.value("dbPassword").toString());
    db.setDatabaseName(systemSettings.value("dbDatabaseName").toString());
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1;CLIENT_SSL=1");

    // load settings from settings.ini
    std::cout << "Loading settings..." << std::endl;

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible )
    {
        if(!db.open())
        {
            std::cout << "ERROR: Unable to open DB, but network is accessible." << std::endl;
//            return -2; //TODO: Show this as error message in app, because if app exits, plain  X server will be shown
        }
        else
        {
            QMLSettingsSingleton::instance()->refetchData();
        }
    }

    // init power manager.
    gPowerManager = new PowerManager(&app);
    gPowerManager->init();

//    QObject::connect( &pwMgr, SIGNAL(powerModeChanged(int)), &viewer, SLOT(powerModeChanged(int)) );

    // create updater
    Updater updater( &app );
    QMetaObject::invokeMethod( &updater, "update", Qt::QueuedConnection );

    // Now we can start listen for events from network manager
    QObject::connect( gNetworkAccessManager, &QNetworkAccessManager::authenticationRequired, &onAuthenticationRequired );
    QObject::connect( gNetworkAccessManager, &QNetworkAccessManager::networkAccessibleChanged, &onNetworkAccessibleChanged );


    // create window to display start up
    QQmlApplicationEngine engine(&app);
    engine.setNetworkAccessManagerFactory( new IviasQmlNetworkAccessManagerFactory() );
    engine.load(QUrl(QStringLiteral("qml/IviasClient/main.qml")));

    QObject *topLevel = engine.rootObjects().value(0);

    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    if ( !window ) {
        qWarning("Error: Your root item has to be a Window.");
        return -1;
    }

    window->show();
    window->showFullScreen();
    window->installEventFilter( new InputActivityFilter(window) );

    return app.exec();
}
