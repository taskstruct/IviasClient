#ifndef PACKAGE_H
#define PACKAGE_H

#include <QtCore/QObject>

#include <QtCore/QDateTime>
#include <QtCore/QString>

class QTemporaryFile;

class Package : public QObject
{
    Q_OBJECT

    enum State {
        Init,
        Unpacking,
        Unpacked,
        InUse,
        Removed,
        Error
    };

    static const QString TitleKey;
    static const QString ThumbnailKey;
    static const QString UIDKey;

public:
    explicit Package( QObject *parent = 0);
    explicit Package( const int adIndex, QObject *parent = 0);
    virtual ~Package();

    int uid() const { return m_uid; }
    const QString title() const;
    const QString thumbnail() const;
    const QString packagePath() const;
    QDateTime lastModified() const;

    void setIndex( int i );
    void setLastModified( QDateTime t );

    void unpack();
    void remove();

    void finish();

    void load();

    bool isValid() { return m_isValid; }

    bool openTmpFile();

signals:
    void packageReady(bool);

public slots:
    void newDataAvailable();
    void unpackFinished(int);

private:

    bool parseMetadata(const QString & pkgPath );

    void recursiveRemove( const QString & dirName );

    State m_state = Init;

    int m_adIndex;
    int m_uid;
    QDateTime m_lastModified = QDateTime();

    QString m_title;
    /// @brief relative path in package for thumbnail picture
    QString m_thumbnail;
    /// @brief absolute path to package e.g /home/user/.IviasClient/Ad1
    QString m_packagePath;
    QString m_tmpPackagePath;

    QTemporaryFile* m_tmpFile = nullptr;

    bool m_isValid = false;
};

#endif // PACKAGE_H
