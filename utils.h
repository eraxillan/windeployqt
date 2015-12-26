/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include "options.h"

QT_BEGIN_NAMESPACE

inline std::wostream &operator<<(std::wostream &str, const QString &s)
{
#ifdef Q_OS_WIN
    str << reinterpret_cast<const wchar_t *>(s.utf16());
#else
    str << s.toStdWString();
#endif
    return str;
}

#ifdef Q_OS_WIN
QString normalizeFileName(const QString &name);
QString winErrorMessage(unsigned long error);
QString findSdkTool(const QString &tool);
#else // !Q_OS_WIN
inline QString normalizeFileName(const QString &name) { return name; }
#endif // !Q_OS_WIN

static const char windowsSharedLibrarySuffix[] = ".dll";
static const char unixSharedLibrarySuffix[] = ".so";

inline QString sharedLibrarySuffix(Platform platform) { return QLatin1String((platform & WindowsBased) ? windowsSharedLibrarySuffix : unixSharedLibrarySuffix); }
bool isBuildDirectory(Platform platform, const QString &dirName);

bool createSymbolicLink(const QFileInfo &source, const QString &target, QString *errorMessage);
bool createDirectory(const QString &directory, QString *errorMessage);
QString findInPath(const QString &file);
QMap<QString, QString> queryQMakeAll(QString *errorMessage);
QString queryQMake(const QString &variable, QString *errorMessage);

inline QString webProcessBinary(const char *binaryName, Platform p)
{
    const QString webProcess = QLatin1String(binaryName);
    return (p & WindowsBased) ? webProcess + QStringLiteral(".exe") : webProcess;
}

inline Platform platformFromMkSpec(const QString &xSpec)
{
    if (xSpec == QLatin1String("linux-g++"))
        return Unix;
    if (xSpec.startsWith(QLatin1String("win32-")))
        return xSpec.contains(QLatin1String("g++")) ? WindowsMinGW : Windows;
    if (xSpec.startsWith(QLatin1String("winrt-x")))
        return WinRtIntel;
    if (xSpec.startsWith(QLatin1String("winrt-arm")))
        return WinRtArm;
    if (xSpec.startsWith(QLatin1String("winphone-x")))
        return WinPhoneIntel;
    if (xSpec.startsWith(QLatin1String("winphone-arm")))
        return WinPhoneArm;
    if (xSpec.startsWith(QLatin1String("wince"))) {
        if (xSpec.contains(QLatin1String("-x86-")))
            return WinCEIntel;
        if (xSpec.contains(QLatin1String("-arm")))
            return WinCEArm;
    }
    return UnknownPlatform;
}

// Return binary from folder
inline QString findBinary(const QString &directory, Platform platform)
{
    QDir dir(QDir::cleanPath(directory));

    const QStringList nameFilters = (platform & WindowsBased) ?
        QStringList(QStringLiteral("*.exe")) : QStringList();
    foreach (const QString &binary, dir.entryList(nameFilters, QDir::Files | QDir::Executable)) {
        if (!binary.contains(QLatin1String(Options::webKitProcessC), Qt::CaseInsensitive)
            && !binary.contains(QLatin1String(Options::webEngineProcessC), Qt::CaseInsensitive)) {
            return dir.filePath(binary);
        }
    }
    return QString();
}

inline QString msgFileDoesNotExist(const QString & file)
{
    return QLatin1Char('"') + QDir::toNativeSeparators(file)
        + QStringLiteral("\" does not exist.");
}

// Simple line wrapping at 80 character boundaries.
inline QString lineBreak(QString s)
{
    for (int i = 80; i < s.size(); i += 80) {
        const int lastBlank = s.lastIndexOf(QLatin1Char(' '), i);
        if (lastBlank >= 0) {
            s[lastBlank] = QLatin1Char('\n');
            i = lastBlank + 1;
        }
    }
    return s;
}

inline QString libraryPath(const QString &libraryLocation, const char *name,
                           const QString &qtLibInfix, Platform platform, bool debug)
{
    QString result = libraryLocation + QLatin1Char('/');
    if (platform & WindowsBased) {
        result += QLatin1String(name);
        result += qtLibInfix;
        if (debug)
            result += QLatin1Char('d');
    } else if (platform & UnixBased) {
        result += QStringLiteral("lib");
        result += QLatin1String(name);
        result += qtLibInfix;
    }
    result += sharedLibrarySuffix(platform);
    return result;
}

inline int qtVersion(const QMap<QString, QString> &qmakeVariables)
{
    const QString versionString = qmakeVariables.value(QStringLiteral("QT_VERSION"));
    const QChar dot = QLatin1Char('.');
    const int majorVersion = versionString.section(dot, 0, 0).toInt();
    const int minorVersion = versionString.section(dot, 1, 1).toInt();
    const int patchVersion = versionString.section(dot, 2, 2).toInt();
    return (majorVersion << 16) | (minorVersion << 8) | patchVersion;
}

// Determine the Qt lib infix from the library path of "Qt5Core<qtblibinfix>[d].dll".
inline QString qtlibInfixFromCoreLibName(const QString &path, bool isDebug, Platform platform)
{
    const int startPos = path.lastIndexOf(QLatin1Char('/')) + 8;
    int endPos = path.lastIndexOf(QLatin1Char('.'));
    if (isDebug && (platform & WindowsBased))
        endPos--;
    return endPos > startPos ? path.mid(startPos, endPos - startPos) : QString();
}

QStringList findSharedLibraries(const QDir &directory, Platform platform,
                                DebugMatchMode debugMatchMode,
                                const QString &prefix = QString());

bool runProcess(const QString &binary, const QStringList &args,
                const QString &workingDirectory = QString(),
                unsigned long *exitCode = 0, QByteArray *stdOut = 0, QByteArray *stdErr = 0,
                QString *errorMessage = 0);

bool readPeExecutable(const QString &peExecutableFileName, QString *errorMessage,
                      QStringList *dependentLibraries = 0, unsigned *wordSize = 0,
                      bool *isDebug = 0, bool isMinGW = false);
bool readElfExecutable(const QString &elfExecutableFileName, QString *errorMessage,
                      QStringList *dependentLibraries = 0, unsigned *wordSize = 0,
                      bool *isDebug = 0);

inline bool readExecutable(const QString &executableFileName, Platform platform,
                           QString *errorMessage, QStringList *dependentLibraries = 0,
                           unsigned *wordSize = 0, bool *isDebug = 0)
{
    return platform == Unix ?
        readElfExecutable(executableFileName, errorMessage, dependentLibraries, wordSize, isDebug) :
        readPeExecutable(executableFileName, errorMessage, dependentLibraries, wordSize, isDebug,
                         (platform == WindowsMinGW));
}

// Return dependent modules of executable files.

inline QStringList findDependentLibraries(const QString &executableFileName, Platform platform, QString *errorMessage)
{
    QStringList result;
    readExecutable(executableFileName, platform, errorMessage, &result);
    return result;
}

QString findD3dCompiler(Platform platform, const QString &qtBinDir, unsigned wordSize);

bool patchQtCore(const QString &path, QString *errorMessage);

extern int optVerboseLevel;

// Helper for recursively finding all dependent Qt libraries.
bool findDependentQtLibraries(const QString &qtBinDir, const QString &binary, Platform platform,
                                     QString *errorMessage, QStringList *result,
                                     unsigned *wordSize = 0, bool *isDebug = 0,
                                     int *directDependencyCount = 0, int recursionDepth = 0);

QT_END_NAMESPACE

#endif // UTILS_H
