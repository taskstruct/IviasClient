#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkReply>
#include <QtQml/qqml.h>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlNetworkAccessManagerFactory>
#include <QtQuick/QQuickWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtWidgets/QApplication>

#include "adsmodel.h"
#include "advertisementslist.h"
#include "clickscounter.h"
#include "globals.h"
#include "helper.h"
#include "inputactivityfilter.h"
//#include "powermanager.h"
#include "qmlsettingssingleton.h"
#include "updater.h"

const QLatin1String IviasClientDBConnection("icdbc");

QNetworkAccessManager *gNetworkAccessManager;
ClicksCounter *gClicksCounter = 0;
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

void showInitMessage(QObject *window, const QString msg)
{
    QString value = window->property("initText").toString();
    value += msg + QLatin1Literal("<br>");
    window->setProperty("initText", value);
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
    QApplication app(argc, argv);
    app.setOrganizationName( "Ivias" );
    app.setOrganizationDomain( "ivias.org" );
    app.setApplicationName( "IviasClient" );
    // GUI
    qmlRegisterType<AdsModel>( "IviasClient", 1, 0, "AdsModel" );;

    qmlRegisterSingletonType<QMLSettingsSingleton>("IviasClient", 1, 0, "IviasSettings", qml_settings_singleton_provider);
    qmlRegisterSingletonType<QMLSettingsSingleton>("IviasClient", 1, 0, "ClickCounter", clicks_counter_singleton_provider);

    // init network manager
    gNetworkAccessManager = new QNetworkAccessManager(&app);

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

    // Read systemSettings.ini
    QSettings systemSettings( Helper::getSystemSettingsPath(), QSettings::IniFormat, &app );
    gIviasClientID = systemSettings.value("iviasClientID").toInt();

    // init DB
    showInitMessage(window, QLatin1Literal("Setup SQL access..."));
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
    showInitMessage(window, QLatin1Literal("Loading settings..."));

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible )
    {
        if(!db.open())
        {
            showInitMessage( window, "ERROR: Unable to open DB, but network is accessible.");
//            return -2; //TODO: Show this as error message in app, because if app exits, plain  X server will be shown
        }
        else
        {
            QMLSettingsSingleton::instance()->refetchData();
        }
    }

//    PowerManager pwMgr( &app );
//    pwMgr.start();

//    QObject::connect( &pwMgr, SIGNAL(powerModeChanged(int)), &viewer, SLOT(powerModeChanged(int)) );

    // create updater
    Updater updater( &app );
    updater.update();

    // Now we can start listen for events from network manager
    QObject::connect( gNetworkAccessManager, &QNetworkAccessManager::authenticationRequired, &onAuthenticationRequired );
    QObject::connect( gNetworkAccessManager, &QNetworkAccessManager::networkAccessibleChanged, &onNetworkAccessibleChanged );

    showInitMessage(window, QLatin1Literal("Done."));

    window->setProperty("initialized", true);

    return app.exec();
}
