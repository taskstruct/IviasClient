#include <QtCore/QSettings>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#include "globals.h"
#include "helper.h"
#include "qmlsettingssingleton.h"

QMLSettingsSingleton::QMLSettingsSingleton(QObject *parent):
    QObject(parent),
    m_projectId(-1),
    m_showSeatbeltWarningEnabled(true),
    m_seatbeltWarningDuration(30*1000), /*30 seconds*/
    m_updateInterval(1*60*60*1000) /*1h*/
{

}

void QMLSettingsSingleton::init()
{
    // get values from local cache
    QSettings settings( Helper::getSettingsPath(), QSettings::IniFormat, this );

    m_projectId = settings.value("projectId", -1).toInt();
    m_showSeatbeltWarningEnabled = settings.value("showSeatbeltWarningEnabled", true).toInt();
    m_seatbeltWarningDuration = settings.value("seatbeltWarningDuration", 30*1000).toInt();
    m_updateInterval = settings.value("updateInterval", 1*60*60*1000).toInt();

    // get project ID from DB
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );

    if( !db.isValid()  || !db.isOpen()) {
        return;
    }

    QSqlQuery query1("SELECT projectId FROM IviasClients WHERE icID = :id", db);
    query1.bindValue(":id", gIviasClientID);

    if( !query1.exec() ) {
        return false;
    }

    if(!query1.next()) {
        return false;
    }

    m_projectId = query1.value(0).toInt();
}

void QMLSettingsSingleton::refetchData()
{
    QSqlDatabase db = QSqlDatabase::database( IviasClientDBConnection );

    if( !db.isValid()  || !db.isOpen() || ( -1 == m_projectId ) ) {
        return;
    }

    QSqlQuery query("SELECT * FROM Projects WHERE projectId = :pid", db);
    query.bindValue(":pid", m_projectId);

    if(!query.exec()) {
        return false;
    }

    // go to first row
    if(!query.next()) {
        return false;
    }

    QSettings settings( Helper::getSettingsPath(), QSettings::IniFormat, this );

    //TODO: Add HTTP setting here

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

    //TODO: titles

    // other properties
    int updateInterval = query.value("updateInterval").toInt();

    if( m_updateInterval != updateInterval ) {
        m_updateInterval = updateInterval;
        settings.setValue("updateInterval", m_updateInterval );
        emit updateIntervalChanged();
    }

    settings.sync();
}
