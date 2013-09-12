#ifndef HELPER_H
#define HELPER_H

#include <QtCore/QString>

class Helper
{
public:
    static const QString dataDir();
    static const QString getSettingsPath();
    static const QString getSystemSettingsPath();
    static const QString tempPackageName();
    static const QString timestampsPath();

private:
    Helper();
};

//inline uint helperConvertChar2Dec( char * arr )
//{
//    return ( (uint)arr[0] | ( (uint)arr[1] << 8 ) | ( (uint)arr[2] << 16 ) | ( (uint)arr[3] << 24 ) );
//}

//inline void helperConvertDec2Char( uint num, char * arr )
//{
//    return ( (uint)arr[0] | ( (uint)arr[1] << 8 ) | ( (uint)arr[2] << 16 ) | ( (uint)arr[3] << 24 ) );

//    arr[0] = num & 0xFF;
//    arr[1] = ( num >> 8  ) & 0xFF;
//    arr[2] = ( num >> 16  ) & 0xFF;
//    arr[3] = ( num >> 24  ) & 0xFF;
//}

#endif // HELPER_H
