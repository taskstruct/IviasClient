#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtQml/qqml.h>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlNetworkAccessManagerFactory>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtWidgets/QApplication>

#include "adsmodel.h"
#include "advertisementslist.h"
#include "clickscounter.h"
#include "configuration.h"
#include "declarativeconfigurationmanager.h"
#include "helper.h"
#include "inputactivityfilter.h"
#include "powermanager.h"
#include "qmlsettingssingleton.h"
#include "updater.h"

struct HttpSettings {
    QUrl url;
    QString username;
    QString password;
//    int port;
};

const QLatin1String IviasClientDBConnection("icdbc");

QNetworkAccessManager *gNetworkAccessManager;
HttpSettings gHttpSettings;
QMLSettingsSingleton *gQmlSettings = 0;
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

    if( !gQmlSettings ) {
        gQmlSettings = new QMLSettingsSingleton();
    }

    return gQmlSettings;
}

void showInitMessage(QObject *window, const QString msg)
{
    QString value = window->property("initText").toString();
    value += msg + QLatin1Literal("<br>");
    window->setProperty("initText", value);
}

bool LoadSettingsFromDB( QSqlDatabase db )
{
    QSqlQuery query1("SELECT projectId FROM IviasClients WHERE icID = :id", db);
    query1.bindValue(":id", gIviasClientID);

    if( !query1.exec() ) {
        return false;
    }

    if(!query1.next()) {
        return false;
    }

    int projectId = query1.value(0).toInt();

    QSqlQuery query2("SELECT * FROM Projects WHERE projectId = :pid", db);
    query2.bindValue(":pid", projectId);

    if(!query2.exec()) {
        return false;
    }

    if(!query2.next()) {
        return false;
    }

    HttpSettings tmp;
    tmp.url = QUrl(query2.value("url").toString());
//    tmp.port = query2.value("port");
    tmp.username = query2.value("username").toString();
    tmp.password = query2.value("password").toString();

    if(tmp.url != gHttpSettings.url) {
        // project has been changed. Remove all data
        AdvertisementsList::instance()->clearList();

        gHttpSettings = tmp;

        //TODO: Start updater to download data
    }
    else {
        gHttpSettings = tmp;
    }

    return true;
}

void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator * authenticator)
{
    if( gHttpSettings.url.isParentOf(reply->request().url()) ) {
        authenticator->setUser(gHttpSettings.username);
        authenticator->setPassword(gHttpSettings.password);
    }
    else {
        qDebug() << "Trying to authenticate with wrong url: " << reply->request().url() <<
                    "\nProject url is: " << gHttpSettings.url;
    }
}

void onNetworkAccessibleChanged ( QNetworkAccessManager::NetworkAccessibility accessible )
{
    if( QNetworkAccessManager::Accessible == accessible )
    {
        QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );

        if( !db.isOpen() ) {
            if(db.open()) {
                LoadSettingsFromDB(db);
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
    qmlRegisterType<AdsModel>( "IviasClient", 1, 0, "AdsModel" );
//TODO: Move as singleton
//    qmlRegisterType<ClicksCounter>( "IviasClient", 1, 0, "ClicksCounter" );
    qmlRegisterType<DeclarativeConfigurationManager>( "IviasClient", 1, 0, "ConfigManager" );

    qmlRegisterSingletonType<QMLSettingsSingleton>("IviasClient", 1, 0, "IviasSettings", qml_settings_singleton_provider);

    // init network manager
    gNetworkAccessManager = new QNetworkAccessManager(&app);
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

    // load settings from settings.ini
    showInitMessage(window, QLatin1Literal("Loading settings..."));
    QSettings settings( Helper::getSettingsPath(), QSettings::IniFormat, &app );
    gHttpSettings.url = QUrl(settings.value("projectUrl").toString());
//    gHttpSettings.port = settings.value("port");
    gHttpSettings.username = settings.value("username").toString();
    gHttpSettings.password = settings.value("password").toString();

    if( gNetworkAccessManager->networkAccessible() == QNetworkAccessManager::Accessible )
    {
        if(!db.open())
        {
            qDebug() << "Unable to open DB, but network is accessible.";
            return -2; //TODO: Show this as error message in app, because if app exits, plain  X server will be shown
        }
        else
        {
            LoadSettingsFromDB(db);
        }
    }
    else
    {
        // load settings from settings.ini
        // Project http address, port, username, password. AdsList should be notificated.
        // QML export: shutdown time?, back to intro time, show seatbelt warning,
        // TODO: Add timer to check for updates in DB. This chekcs should be done before checks for packages. This can be done from updater
    }

//    DeclarativeView viewer;

    PowerManager pwMgr( &app );
    pwMgr.start();

//    QObject::connect( &pwMgr, SIGNAL(powerModeChanged(int)), &viewer, SLOT(powerModeChanged(int)) );

    // create updater
//    Updater updater( &app );
//    updater.update();

//    viewer.show();

    showInitMessage(window, QLatin1Literal("Done."));

    window->setProperty("initialized", true);

    return app.exec();
}
