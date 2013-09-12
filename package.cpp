#include "package.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfoList>
#include <QtCore/QProcess>
#include <QtCore/QSettings>

#include <QtCore/QDebug>

#include "constraints.h"
#include "helper.h"

const QString Package::thumbnailKey = QLatin1Literal("thumbnail");
const QString Package::startPageKey = QLatin1Literal("startpage");

Package::Package( const int adIndex, QObject *parent) :
    QObject(parent),
    m_state( Init ),
    m_adIndex(adIndex),
    m_lastModified( QDateTime() ),
    m_isValid(false)
{
    m_packagePath = Helper::dataDir() + QString(QLatin1Literal("Ad%1/")).arg(m_adIndex);

    Q_ASSERT( m_adIndex < cTotalNumberOfAds );
}

Package::~Package()
{
    //TODO: Remove everyting that might be working
}

const QString Package::thumbnail() const
{
    return m_packagePath + m_thumbnail;
}

const QString Package::startFile() const
{
    return m_packagePath + m_startFile;
}

const QString Package::packagePath() const
{
    return m_packagePath;
}

QDateTime Package::lastModified() const
{
    return m_lastModified;
}

void Package::setIndex( int i )
{
    m_adIndex = i;

    Q_ASSERT( i < cTotalNumberOfAds );
}

void Package::setLastModified( QDateTime t )
{
    m_lastModified = t;
}

void Package::unpack()
{
    QProcess *sevenZip = new QProcess(this);

    QStringList args;

    args << "x" << Helper::tempPackageName() << "-o" + Helper::dataDir() + "tmpdir";

    qDebug() << "7z args: " << args;

    sevenZip->start( "/usr/bin/7z", args );

    m_state = Unpacking;

    connect( sevenZip, SIGNAL(finished(int)), this, SLOT(unpackFinished(int)) );

//    sevenZip->deleteLater();
}

void Package::unpackFinished( int exitCode )
{
    //clean up tmp file
    QFile::remove( Helper::tempPackageName() );

    if ( exitCode != 0 )
    {
        m_state = Error;
        m_isValid = false;
        emit packageReady(false);
        return;
    }

    m_state = Unpacked;

    QDir dataDir( Helper::dataDir() + "tmpdir" );
    QFileInfoList list = dataDir.entryInfoList( QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot );

    if( list.length() != 1 )
    {
        // there should be only one dirrectory
        m_isValid = false;
        emit packageReady(false);
        return;
    }

    m_tmpPackagePath = list.at(0).absoluteFilePath();

    qDebug() << "Searching for .metadata in " << m_tmpPackagePath;

    bool ok = parseMetadata( m_tmpPackagePath + "/.metadata" );

    if(ok) {
        m_isValid = true;
    }
    else {
        m_isValid = false;
    }

    emit packageReady(ok);
}

void Package::remove()
{
    m_state = Removed;

    recursiveRemove( m_packagePath );

    qDebug() << "Removing package ... " << m_packagePath;

    QDir d;
    bool res = d.rmdir( m_packagePath );

    qDebug() << "Removing Ad folder " << m_packagePath << " ... " << res;

    m_thumbnail = "";
    m_startFile = "";
    m_packagePath = "";
    m_isValid = false;
}

void Package::recursiveRemove(const QString &dirName)
{
    QDir dir( dirName );
    QFileInfoList list = dir.entryInfoList( QDir::Dirs | QDir::Files | QDir::Hidden |  QDir::NoDotAndDotDot );
    QFileInfo d;

    foreach(d, list)
    {
        if ( d.isFile() )
        {
            qDebug() << "Removing file " << d.absoluteFilePath();
            QFile::remove( d.absoluteFilePath() );
        }
        else if ( d.isDir() )
        {
            // remove all files and dirs in this dir
            recursiveRemove( d.absoluteFilePath() );

            // remove dir. It should be empty
            qDebug() << "Removing directory " << d.absoluteFilePath();
            dir.rmdir( d.absolutePath() );
        } else {
            Q_ASSERT_X( false, "Package::recursiveRemove()", "Trying to remove something that is not directory or file" );
        }
    }
}


bool Package::parseMetadata( const QString & fileName )
{
    if( !QFile::exists(fileName) ) {
        return false;
    }

    QSettings metadata( fileName, QSettings::IniFormat );

    m_thumbnail = metadata.value( thumbnailKey, "" ).toString();
    m_startFile = metadata.value( startPageKey, "" ).toString();

    bool ok = ( ( m_thumbnail != "" ) && ( m_startFile != "" ) );

    if(!ok) m_state = Error;

    return ok;
}

void Package::finish()
{
    // rename package dir from tmpdir to Ad#
    QDir pkgDir;

    qDebug() << "Renaming " << m_tmpPackagePath << " to " << m_packagePath;

    bool res = pkgDir.rename( m_tmpPackagePath, m_packagePath );

    pkgDir.rmdir( Helper::dataDir() + "tmpdir" );

    qDebug() << "Ranamed status " << res;

    // update timestamps
    QSettings s( Helper::timestampsPath() );

    const QString key = QLatin1String("Ad") + QString::number( m_adIndex );

    s.setValue( key, m_lastModified );

    qDebug() << "Saving package[" << m_adIndex << "] with timestamp = " << m_lastModified;
}

void Package::load()
{
    QDir d(m_packagePath);

    if(!d.exists()) {
        m_isValid = false;
        return;
    }

    if(!parseMetadata( m_packagePath + ".metadata" )) {
        m_isValid = false;
        return;
    }

    QSettings s( Helper::timestampsPath() );

    const QString key = QLatin1String("Ad") + QString::number( m_adIndex );

    m_lastModified = s.value( key, QDateTime() ).toDateTime();

    m_isValid = true;

//    qDebug() << "Loading package[" << m_adIndex << "] with timestamp = " << m_lastModified;
}
