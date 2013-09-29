#include <QtCore/QSettings>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "constraints.h"
#include "globals.h"
#include "helper.h"
#include "qmlsettingssingleton.h"

QMLSettingsSingleton* QMLSettingsSingleton::s_instance = 0;

QMLSettingsSingleton* QMLSettingsSingleton::instance(QObject *parent)
{
    if( !s_instance ) {
        s_instance = new QMLSettingsSingleton(parent);
    }

    return s_instance;
}

QMLSettingsSingleton::QMLSettingsSingleton(QObject *parent):
    QObject(parent),
    m_projectId(-1),
    m_url(),
    m_username(),
    m_password(),
    m_port(80),
    m_showSeatbeltWarningEnabled(true),
    m_seatbeltWarningDuration(30*1000), /*30 seconds*/
    m_updateInterval(1*60*60*1000) /*1h*/
{
    m_titles.reserve(cNumberOfPages);

    // get values from local cache
    QSettings settings( Helper::getSettingsPath(), QSettings::IniFormat, this );

    m_projectId = settings.value("projectId").toInt();

    m_url = QUrl(settings.value("projectUrl").toString());
    m_port = settings.value("port", 80).toInt();
    m_username = settings.value("username").toString();
    m_password = settings.value("password").toString();

    m_projectId = settings.value("projectId", -1).toInt();
    m_showSeatbeltWarningEnabled = settings.value("showSeatbeltWarningEnabled", true).toInt();
    m_seatbeltWarningDuration = settings.value("seatbeltWarningDuration", 30*1000).toInt();
    m_titles = settings.value("titles", QStringList{"Page 1", "Page 2", "Page 3", "Page 4", "Page 5", "Page 6"}).toStringList();
    m_updateInterval = settings.value("updateInterval", 1*60*60*1000).toInt();
}

void QMLSettingsSingleton::fetchProjectId(QSqlDatabase &db)
{
    QSqlQuery query("SELECT projectId FROM IviasClients WHERE icID = :id", db);
    query.bindValue(":id", gIviasClientID);

    if( !query.exec() ) {
        return;
    }

    if(!query.next()) {
        if( -1 != m_projectId )
        {
            m_projectId = -1;
            emit projectChanged();
        }
        return;
    }

    int prjId = query.value(0).toInt();

    if( prjId != m_projectId )
    {
        m_projectId = prjId;
        emit projectChanged();
    }
}

void QMLSettingsSingleton::refetchData()
{
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );

    if( !db.isValid()  || !db.isOpen() ) {
        return;
    }

    fetchProjectId(db);

    if( -1 == m_projectId ) {
        return;
    }

    QSqlQuery query("SELECT * FROM Projects WHERE projectId = :pid", db);
    query.bindValue(":pid", m_projectId);

    if(!query.exec()) {
        return;
    }

    // go to first row
    if(!query.next()) {
        // there is no project with this id. Error
        // TODO: Error!
        return;
    }

    QSettings settings( Helper::getSettingsPath(), QSettings::IniFormat, this );

    // HTTP settings
    QString urlString = query.value("url").toString();
    QUrl url(urlString);
    if( url != m_url ) {
        m_url = url;
        settings.setValue("projectUrl", urlString);
        emit projectChanged();
    }

    // just save username and passwork. If they changed, network access manager will require them
    m_username = query.value("username").toString();
    settings.setValue("username", m_username);
    m_password = query.value("password").toString();
    settings.setValue("password", m_password);

    m_port = query.value("port").toInt();
    settings.setValue("port", m_port);

    // QML exported properties
    bool showSeatbeltWarningEnabled = query.value("showSeatbeltWarningEnabled").toBool();

    if( m_showSeatbeltWarningEnabled != showSeatbeltWarningEnabled ) {
        m_showSeatbeltWarningEnabled = showSeatbeltWarningEnabled;
        settings.setValue("showSeatbeltWarningEnabled", m_showSeatbeltWarningEnabled );
        emit showSeatbeltWarningEnabledChanged();
    }

    int seatbeltWarningDuration = query.value("seatbeltWarningDuration").toInt();

    if( m_seatbeltWarningDuration != seatbeltWarningDuration ) {
        m_seatbeltWarningDuration = seatbeltWarningDuration;
        settings.setValue("seatbeltWarningDuration", m_seatbeltWarningDuration );
        emit seatbeltWarningDurationChanged();
    }

    QString titlesString = query.value("titles").toString();
    QStringList titles = titlesString.split(';');

    if( titles.length() == cNumberOfPages  )
    {
        // compare titles one by one
        for( int i = 0; i < cNumberOfPages; i++ )
        {
            if( m_titles[i] != titles[i] )
            {
                m_titles = titles;
                settings.setValue("titles", m_titles );
                emit titlesChanged();
                break;
            }
        }
    }
    else
    {
        // titles should be exactly cNumberOfPages
        Q_ASSERT(false);
    }

    // other settings
    int updateInterval = query.value("updateInterval").toInt();

    if( m_updateInterval != updateInterval ) {
        m_updateInterval = updateInterval;
        settings.setValue("updateInterval", m_updateInterval );
        emit updateIntervalChanged();
    }

    settings.sync();
}
