#include "package.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfoList>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryFile>

#include <QtNetwork/QNetworkReply>

#include <QtCore/QDebug>

#include "constraints.h"
#include "helper.h"

const QString Package::TitleKey = QLatin1Literal("title");
const QString Package::ThumbnailKey = QLatin1Literal("thumbnail");
const QString Package::UIDKey = QLatin1Literal("uid");

Package::Package( const int adIndex, QObject *parent) :
    QObject(parent),
    m_adIndex(adIndex)
{
    m_packagePath = Helper::dataDir() + QString(QLatin1Literal("Ad%1/")).arg(m_adIndex);

    Q_ASSERT( m_adIndex < cTotalNumberOfAds );
}

Package::~Package()
{
    //TODO: Remove everyting that might be working

    if( m_tmpFile ) {
        delete m_tmpFile;
    }
}

const QString Package::title() const
{
    return m_title;
}

const QString Package::thumbnail() const
{
    return m_packagePath + m_thumbnail;
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

    args << "x" << QFileInfo(*m_tmpFile).absoluteFilePath() << "-o" + Helper::dataDir() + "tmpdir";

    qDebug() << "7z args: " << args;

    sevenZip->start( "/usr/bin/7z", args );

    m_state = Unpacking;

    connect( sevenZip, SIGNAL(finished(int)), this, SLOT(unpackFinished(int)) );
}

void Package::newDataAvailable()
{
    QNetworkReply * reply = qobject_cast<QNetworkReply *>( sender() );

    Q_ASSERT( 0 == reply );

    m_tmpFile->write( reply->readAll() );
}

void Package::unpackFinished( int exitCode )
{
    QObject *sevenZipProc = sender();

    if(sevenZipProc) {
        sevenZipProc->deleteLater();
    }

    //clean up tmp file
    delete m_tmpFile;
    m_tmpFile = nullptr;

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

    m_tmpPackagePath = list.at(0).absoluteFilePath() + QChar('/');

    qDebug() << "Searching for .metadata in " << m_tmpPackagePath;

    bool ok = parseMetadata( m_tmpPackagePath );

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

    m_title.clear();
    m_thumbnail.clear();
    m_packagePath.clear();
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


bool Package::parseMetadata( const QString & pkgPath )
{
    QString metadataFileName = pkgPath + ".metadata";

    if( !QFile::exists(metadataFileName) ) {
        return false;
    }

    QSettings metadata( metadataFileName, QSettings::IniFormat );

    m_title = metadata.value( TitleKey, "Title" ).toString();
    m_thumbnail = metadata.value( ThumbnailKey, "" ).toString();
    m_uid = metadata.value( UIDKey, 0 ).toInt();

    bool ok = ( !m_thumbnail.isEmpty() && QFile::exists( pkgPath + "main.qml" ) && ( m_uid != 0 ) );

    if(!ok) {
        m_state = Error;
    }

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

//    qDebug() << "Saving package[" << m_adIndex << "] with timestamp = " << m_lastModified;
}

void Package::load()
{
    QDir d(m_packagePath);

    if(!d.exists()) {
        m_isValid = false;
        return;
    }

    if(!parseMetadata( m_packagePath )) {
        m_isValid = false;
        return;
    }

    QSettings s( Helper::timestampsPath() );

    const QString key = QLatin1String("Ad") + QString::number( m_adIndex );

    m_lastModified = s.value( key, QDateTime() ).toDateTime();

    m_isValid = true;

//    qDebug() << "Loading package[" << m_adIndex << "] with timestamp = " << m_lastModified;
}

bool Package::openTmpFile()
{
    QString templateName = QDir::tempPath();

    if( !templateName.endsWith( '/' ) )
    {
        templateName += '/';
    }

    templateName += QLatin1String("ivias_tmp_XXXXXX");

    m_tmpFile = new QTemporaryFile( templateName );

    if( m_tmpFile->open() )
    {
        return true;
    }
    else
    {
        delete m_tmpFile;
    }
}
