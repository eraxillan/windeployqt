#ifndef TYPES_H
#define TYPES_H

#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QSharedPointer>
#include <QtCore/QVector>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

#include <iostream>

QT_BEGIN_NAMESPACE

// utils.h types
enum PlatformFlag {
    WindowsBased = 0x1000,
    UnixBased = 0x2000,
    IntelBased = 0x4000,
    ArmBased = 0x8000,
    MinGW = 0x10000
};

enum Platform {
    Windows = WindowsBased + IntelBased,
    WindowsMinGW = WindowsBased + IntelBased + MinGW,
    WinRtIntel = WindowsBased + IntelBased + 1,
    WinRtArm = WindowsBased + ArmBased + 2,
    WinPhoneIntel = WindowsBased + IntelBased + 3,
    WinPhoneArm = WindowsBased + ArmBased + 4,
    WinCEIntel = WindowsBased + IntelBased + 5,
    WinCEArm = WindowsBased + ArmBased + 6,
    Unix = UnixBased,
    UnknownPlatform
};

enum ListOption {
    ListNone = 0,
    ListSource,
    ListTarget,
    ListRelative,
    ListMapping
};

enum DebugMatchMode {
    MatchDebug,
    MatchRelease,
    MatchDebugOrRelease
};

// Recursively update a file or directory, matching DirectoryFileEntryFunction against the QDir
// to obtain the files.
enum UpdateFileFlag  {
    ForceUpdateFile = 0x1,
    SkipUpdateFile = 0x2,
    RemoveEmptyQmlDirectories = 0x4
};

QT_END_NAMESPACE

#endif // TYPES_H

