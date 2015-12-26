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

#include "deployment.h"
#include "jsonoutput.h"
#include "utils.h"
#include "qtmodules.h"
#include "qmlutils.h"

QT_BEGIN_NAMESPACE

// Base class to filter files by name filters functions to be passed to updateFile().
class NameFilterFileEntryFunction {
public:
    explicit NameFilterFileEntryFunction(const QStringList &nameFilters);
    QStringList operator()(const QDir &dir) const;

private:
    const QStringList m_nameFilters;
};

// Convenience for all files.

// Base class to filter debug/release Windows DLLs for functions to be passed to updateFile().
// Tries to pre-filter by namefilter and does check via PE.
class DllDirectoryFileEntryFunction {
public:
    explicit DllDirectoryFileEntryFunction(Platform platform, DebugMatchMode debugMatchMode, const QString &prefix = QString());

    QStringList operator()(const QDir &dir) const;

private:
    const Platform m_platform;
    const DebugMatchMode m_debugMatchMode;
    const QString m_prefix;
};

// File entry filter function for updateFile() that returns a list of files for
// QML import trees: DLLs (matching debgug) and .qml/,js, etc.
class QmlDirectoryFileEntryFunction {
public:
    explicit QmlDirectoryFileEntryFunction(Platform platform, DebugMatchMode debugMatchMode, bool skipQmlSources = false);

    QStringList operator()(const QDir &dir) const;

private:
    static inline QStringList qmlNameFilters(bool skipQmlSources);

    NameFilterFileEntryFunction m_qmlNameFilter;
    DllDirectoryFileEntryFunction m_dllFilter;
};

//-----------------------------------------------------------------------------

DllDirectoryFileEntryFunction::DllDirectoryFileEntryFunction(Platform platform, DebugMatchMode debugMatchMode, const QString &prefix) :
    m_platform(platform), m_debugMatchMode(debugMatchMode), m_prefix(prefix)
{}

QStringList DllDirectoryFileEntryFunction::operator()(const QDir &dir) const
{
    return findSharedLibraries(dir, m_platform, m_debugMatchMode, m_prefix);
}

QmlDirectoryFileEntryFunction::QmlDirectoryFileEntryFunction(Platform platform, DebugMatchMode debugMatchMode, bool skipQmlSources)
    : m_qmlNameFilter(QmlDirectoryFileEntryFunction::qmlNameFilters(skipQmlSources))
    , m_dllFilter(platform, debugMatchMode)
{}

QStringList QmlDirectoryFileEntryFunction::operator()(const QDir &dir) const
{
    return m_dllFilter(dir) + m_qmlNameFilter(dir);
}

QStringList QmlDirectoryFileEntryFunction::qmlNameFilters(bool skipQmlSources)
{
    QStringList result;
    result << QStringLiteral("qmldir") << QStringLiteral("*.qmltypes");
    if (!skipQmlSources)
        result << QStringLiteral("*.js") <<  QStringLiteral("*.qml") << QStringLiteral("*.png");
    return result;
}

NameFilterFileEntryFunction::NameFilterFileEntryFunction(const QStringList &nameFilters) :
    m_nameFilters(nameFilters)
{}

QStringList NameFilterFileEntryFunction::operator()(const QDir &dir) const
{
    return dir.entryList(m_nameFilters, QDir::Files);
}

//-----------------------------------------------------------------------------

static inline QString webProcessBinary(const char *binaryName, Platform p)
{
    const QString webProcess = QLatin1String(binaryName);
    return (p & WindowsBased) ? webProcess + QStringLiteral(".exe") : webProcess;
}

// Determine the Qt lib infix from the library path of "Qt5Core<qtblibinfix>[d].dll".
static QString qtlibInfixFromCoreLibName(const QString &path, bool isDebug, Platform platform)
{
    const int startPos = path.lastIndexOf(QLatin1Char('/')) + 8;
    int endPos = path.lastIndexOf(QLatin1Char('.'));
    if (isDebug && (platform & WindowsBased))
        endPos--;
    return endPos > startPos ? path.mid(startPos, endPos - startPos) : QString();
}

static QString libraryPath(const QString &libraryLocation, const char *name, const QString &qtLibInfix, Platform platform, bool debug)
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

static bool updateFile(const QString &sourceFileName, const QStringList &nameFilters,
                const QString &targetDirectory, unsigned flags, JsonOutput *json, QString *errorMessage)
{
    const QFileInfo sourceFileInfo(sourceFileName);
    const QString targetFileName = targetDirectory + QLatin1Char('/') + sourceFileInfo.fileName();
    if (optVerboseLevel > 1)
        std::wcout << "Checking " << sourceFileName << ", " << targetFileName<< '\n';

    if (!sourceFileInfo.exists()) {
        *errorMessage = QString::fromLatin1("%1 does not exist.").arg(QDir::toNativeSeparators(sourceFileName));
        return false;
    }

    if (sourceFileInfo.isSymLink()) {
        *errorMessage = QString::fromLatin1("Symbolic links are not supported (%1).")
                        .arg(QDir::toNativeSeparators(sourceFileName));
        return false;
    }

    const QFileInfo targetFileInfo(targetFileName);

    if (sourceFileInfo.isDir()) {
        if (targetFileInfo.exists()) {
            if (!targetFileInfo.isDir()) {
                *errorMessage = QString::fromLatin1("%1 already exists and is not a directory.")
                                .arg(QDir::toNativeSeparators(targetFileName));
                return false;
            } // Not a directory.
        } else { // exists.
            QDir d(targetDirectory);
            if (optVerboseLevel)
                std::wcout << "Creating " << QDir::toNativeSeparators(targetFileName) << ".\n";
            if (!(flags & SkipUpdateFile) && !d.mkdir(sourceFileInfo.fileName())) {
                *errorMessage = QString::fromLatin1("Cannot create directory %1 under %2.")
                                .arg(sourceFileInfo.fileName(), QDir::toNativeSeparators(targetDirectory));
                return false;
            }
        }
        // Recurse into directory
        QDir dir(sourceFileName);
        const QStringList allEntries = dir.entryList(nameFilters, QDir::Files) + dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString &entry, allEntries)
            if (!updateFile(sourceFileName + QLatin1Char('/') + entry, nameFilters, targetFileName, flags, json, errorMessage))
                return false;
        return true;
    } // Source is directory.

    if (targetFileInfo.exists()) {
        if (!(flags & ForceUpdateFile)
            && targetFileInfo.lastModified() >= sourceFileInfo.lastModified()) {
            if (optVerboseLevel)
                std::wcout << sourceFileInfo.fileName() << " is up to date.\n";
            if (json)
                json->addFile(sourceFileName, targetDirectory);
            return true;
        }
        QFile targetFile(targetFileName);
        if (!(flags & SkipUpdateFile) && !targetFile.remove()) {
            *errorMessage = QString::fromLatin1("Cannot remove existing file %1: %2")
                            .arg(QDir::toNativeSeparators(targetFileName), targetFile.errorString());
            return false;
        }
    } // target exists
    QFile file(sourceFileName);
    if (optVerboseLevel)
        std::wcout << "Updating " << sourceFileInfo.fileName() << ".\n";
    if (!(flags & SkipUpdateFile) && !file.copy(targetFileName)) {
        *errorMessage = QString::fromLatin1("Cannot copy %1 to %2: %3")
                .arg(QDir::toNativeSeparators(sourceFileName),
                     QDir::toNativeSeparators(targetFileName),
                     file.errorString());
        return false;
    }
    if (json)
        json->addFile(sourceFileName, targetDirectory);
    return true;
}

template <class DirectoryFileEntryFunction>
static bool updateFile(const QString &sourceFileName,
                DirectoryFileEntryFunction directoryFileEntryFunction,
                const QString &targetDirectory,
                unsigned flags,
                JsonOutput *json,
                QString *errorMessage)
{
    const QFileInfo sourceFileInfo(sourceFileName);
    const QString targetFileName = targetDirectory + QLatin1Char('/') + sourceFileInfo.fileName();
    if (optVerboseLevel > 1)
        std::wcout << "Checking " << sourceFileName << ", " << targetFileName << '\n';

    if (!sourceFileInfo.exists()) {
        *errorMessage = QString::fromLatin1("%1 does not exist.").arg(QDir::toNativeSeparators(sourceFileName));
        return false;
    }

    const QFileInfo targetFileInfo(targetFileName);

    if (sourceFileInfo.isSymLink()) {
        const QString sourcePath = sourceFileInfo.symLinkTarget();
        const QString relativeSource = QDir(sourceFileInfo.absolutePath()).relativeFilePath(sourcePath);
        if (relativeSource.contains(QLatin1Char('/'))) {
            *errorMessage = QString::fromLatin1("Symbolic links across directories are not supported (%1).")
                            .arg(QDir::toNativeSeparators(sourceFileName));
            return false;
        }

        // Update the linked-to file
        if (!updateFile(sourcePath, directoryFileEntryFunction, targetDirectory, flags, json, errorMessage))
            return false;

        if (targetFileInfo.exists()) {
            if (!targetFileInfo.isSymLink()) {
                *errorMessage = QString::fromLatin1("%1 already exists and is not a symbolic link.")
                                .arg(QDir::toNativeSeparators(targetFileName));
                return false;
            } // Not a symlink
            const QString relativeTarget = QDir(targetFileInfo.absolutePath()).relativeFilePath(targetFileInfo.symLinkTarget());
            if (relativeSource == relativeTarget) // Exists and points to same entry: happy.
                return true;
            QFile existingTargetFile(targetFileName);
            if (!(flags & SkipUpdateFile) && !existingTargetFile.remove()) {
                *errorMessage = QString::fromLatin1("Cannot remove existing symbolic link %1: %2")
                                .arg(QDir::toNativeSeparators(targetFileName), existingTargetFile.errorString());
                return false;
            }
        } // target symbolic link exists
        return createSymbolicLink(QFileInfo(targetDirectory + QLatin1Char('/') + relativeSource), sourceFileInfo.fileName(), errorMessage);
    } // Source is symbolic link

    if (sourceFileInfo.isDir()) {
        bool created = false;
        if (targetFileInfo.exists()) {
            if (!targetFileInfo.isDir()) {
                *errorMessage = QString::fromLatin1("%1 already exists and is not a directory.")
                                .arg(QDir::toNativeSeparators(targetFileName));
                return false;
            } // Not a directory.
        } else { // exists.
            QDir d(targetDirectory);
            if (optVerboseLevel)
                std::wcout << "Creating " << targetFileName << ".\n";
            if (!(flags & SkipUpdateFile)) {
                created = d.mkdir(sourceFileInfo.fileName());
                if (!created) {
                    *errorMessage = QString::fromLatin1("Cannot create directory %1 under %2.")
                            .arg(sourceFileInfo.fileName(), QDir::toNativeSeparators(targetDirectory));
                    return false;
                }
            }
        }
        // Recurse into directory
        QDir dir(sourceFileName);

        const QStringList allEntries = directoryFileEntryFunction(dir) + dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString &entry, allEntries)
            if (!updateFile(sourceFileName + QLatin1Char('/') + entry, directoryFileEntryFunction, targetFileName, flags, json, errorMessage))
                return false;
        // Remove empty directories, for example QML import folders for which the filter did not match.
        if (created && (flags & RemoveEmptyQmlDirectories)) {
            QDir d(targetFileName);
            const QStringList entries = d.entryList(QStringList(), QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
            if (entries.isEmpty() || (entries.size() == 1 && entries.first() == QLatin1String("qmldir"))) {
                if (!d.removeRecursively()) {
                    *errorMessage = QString::fromLatin1("Cannot remove empty directory %1.")
                            .arg(QDir::toNativeSeparators(targetFileName));
                    return false;
                }
                if (json)
                    json->removeTargetDirectory(targetFileName);
            }
        }
        return true;
    } // Source is directory.

    if (targetFileInfo.exists()) {
        if (!(flags & ForceUpdateFile)
            && targetFileInfo.lastModified() >= sourceFileInfo.lastModified()) {
            if (optVerboseLevel)
                std::wcout << sourceFileInfo.fileName() << " is up to date.\n";
            if (json)
                json->addFile(sourceFileName, targetDirectory);
            return true;
        }
        QFile targetFile(targetFileName);
        if (!(flags & SkipUpdateFile) && !targetFile.remove()) {
            *errorMessage = QString::fromLatin1("Cannot remove existing file %1: %2")
                            .arg(QDir::toNativeSeparators(targetFileName), targetFile.errorString());
            return false;
        }
    } // target exists
    QFile file(sourceFileName);
    if (optVerboseLevel)
        std::wcout << "Updating " << sourceFileInfo.fileName() << ".\n";
    if (!(flags & SkipUpdateFile)) {
        if (!file.copy(targetFileName)) {
            *errorMessage = QString::fromLatin1("Cannot copy %1 to %2: %3")
                .arg(QDir::toNativeSeparators(sourceFileName),
                     QDir::toNativeSeparators(targetFileName),
                     file.errorString());
            return false;
        }
        if (!(file.permissions() & QFile::WriteUser)) { // QTBUG-40152, clear inherited read-only attribute
            QFile targetFile(targetFileName);
            if (!targetFile.setPermissions(targetFile.permissions() | QFile::WriteUser)) {
                *errorMessage = QString::fromLatin1("Cannot set write permission on %1: %2")
                    .arg(QDir::toNativeSeparators(targetFileName), file.errorString());
                return false;
            }
        } // Check permissions
    } // !SkipUpdateFile
    if (json)
        json->addFile(sourceFileName, targetDirectory);
    return true;
}

static bool updateFile(const QString &sourceFileName, const QString &targetDirectory,
                       unsigned flags, JsonOutput *json, QString *errorMessage)
{
    return updateFile(sourceFileName, NameFilterFileEntryFunction(QStringList()),
                      targetDirectory, flags, json, errorMessage);
}

// Return dependent modules of executable files.
static QStringList findDependentLibraries(const QString &executableFileName, Platform platform, QString *errorMessage)
{
    QStringList result;
    readExecutable(executableFileName, platform, errorMessage, &result);
    return result;
}

// Helper for recursively finding all dependent Qt libraries.
static bool findDependentQtLibraries(const QString &qtBinDir, const QString &binary, Platform platform,
                                     QString *errorMessage, QStringList *result,
                                     unsigned *wordSize = 0, bool *isDebug = 0,
                                     int *directDependencyCount = 0, int recursionDepth = 0)
{
    QStringList dependentLibs;
    if (directDependencyCount)
        *directDependencyCount = 0;
    if (!readExecutable(binary, platform, errorMessage, &dependentLibs, wordSize, isDebug)) {
        errorMessage->prepend(QLatin1String("Unable to find dependent libraries of ") +
                              QDir::toNativeSeparators(binary) + QLatin1String(" :"));
        return false;
    }
    // Filter out the Qt libraries. Note that depends.exe finds libs from optDirectory if we
    // are run the 2nd time (updating). We want to check against the Qt bin dir libraries
    const int start = result->size();
    foreach (const QString &lib, dependentLibs) {
        if (isQtModule(lib)) {
            const QString path = normalizeFileName(qtBinDir + QLatin1Char('/') + QFileInfo(lib).fileName());
            if (!result->contains(path))
                result->append(path);
        }
    }
    const int end = result->size();
    if (directDependencyCount)
        *directDependencyCount = end - start;
    // Recurse
    for (int i = start; i < end; ++i)
        if (!findDependentQtLibraries(qtBinDir, result->at(i), platform, errorMessage, result, 0, 0, 0, recursionDepth + 1))
            return false;
    return true;
}

static QStringList findQtPlugins(quint64 *usedQtModules, quint64 disabledQtModules,
                                 const QString &qtPluginsDirName, const QString &libraryLocation,
                                 DebugMatchMode debugMatchModeIn, Platform platform, QString *platformPlugin)
{
    QString errorMessage;
    if (qtPluginsDirName.isEmpty())
        return QStringList();
    QDir pluginsDir(qtPluginsDirName);
    QStringList result;
    foreach (const QString &subDirName, pluginsDir.entryList(QStringList(QLatin1String("*")), QDir::Dirs | QDir::NoDotAndDotDot)) {
        const quint64 module = qtModuleForPlugin(subDirName);
        if (module & *usedQtModules) {
            const DebugMatchMode debugMatchMode = (module & QtWebEngineCoreModule)
                    ? MatchDebugOrRelease // QTBUG-44331: Debug detection does not work for webengine, deploy all.
                    : debugMatchModeIn;
            const QString subDirPath = qtPluginsDirName + QLatin1Char('/') + subDirName;
            QDir subDir(subDirPath);
            // Filter out disabled plugins
            if (disabledQtModules & QtQmlToolingModule && subDirName == QLatin1String("qmltooling"))
                continue;
            // Filter for platform or any.
            QString filter;
            const bool isPlatformPlugin = subDirName == QLatin1String("platforms");
            if (isPlatformPlugin) {
                switch (platform) {
                case Windows:
                case WindowsMinGW:
                case WinCEIntel:
                case WinCEArm:
                    filter = QStringLiteral("qwindows");
                    break;
                case WinRtIntel:
                case WinRtArm:
                case WinPhoneIntel:
                case WinPhoneArm:
                    filter = QStringLiteral("qwinrt");
                    break;
                case Unix:
                    filter = QStringLiteral("libqxcb");
                    break;
                case UnknownPlatform:
                    break;
                }
            } else {
                filter  = QLatin1String("*");
            }
            const QStringList plugins = findSharedLibraries(subDir, platform, debugMatchMode, filter);
            foreach (const QString &plugin, plugins) {
                const QString pluginPath = subDir.absoluteFilePath(plugin);
                if (isPlatformPlugin)
                    *platformPlugin = pluginPath;
                QStringList dependentQtLibs;
                quint64 neededModules = 0;
                if (findDependentQtLibraries(libraryLocation, pluginPath, platform, &errorMessage, &dependentQtLibs)) {
                    for (int d = 0; d < dependentQtLibs.size(); ++ d)
                        neededModules |= qtModule(dependentQtLibs.at(d));
                } else {
                    std::wcerr << "Warning: Cannot determine dependencies of "
                               << QDir::toNativeSeparators(pluginPath) << ": " << errorMessage << '\n';
                }
                if (neededModules & disabledQtModules) {
                    if (optVerboseLevel)
                        std::wcout << "Skipping plugin " << plugin << " due to disabled dependencies.\n";
                } else {
                    if (const quint64 missingModules = (neededModules & ~*usedQtModules)) {
                        *usedQtModules |= missingModules;
                        if (optVerboseLevel)
                            std::wcout << "Adding " << formatQtModules(missingModules).constData() << " for " << plugin << '\n';
                    }
                    result.append(pluginPath);
                }
            } // for filter
        } // type matches
    } // for plugin folder
    return result;
}

// Search for "qt_prfxpath=xxxx" in \a path, and replace it with "qt_prfxpath=."
static bool patchQtCore(const QString &path, QString *errorMessage)
{
    if (optVerboseLevel)
        std::wcout << "Patching " << QFileInfo(path).fileName() << "...\n";

    QFile file(path);
    if (!file.open(QIODevice::ReadWrite)) {
        *errorMessage = QString::fromLatin1("Unable to patch %1: %2").arg(
                    QDir::toNativeSeparators(path), file.errorString());
        return false;
    }
    QByteArray content = file.readAll();

    if (content.isEmpty()) {
        *errorMessage = QString::fromLatin1("Unable to patch %1: Could not read file content").arg(
                    QDir::toNativeSeparators(path));
        return false;
    }

    QByteArray prfxpath("qt_prfxpath=");
    int startPos = content.indexOf(prfxpath);
    if (startPos == -1) {
        *errorMessage = QString::fromLatin1(
                    "Unable to patch %1: Could not locate pattern \"qt_prfxpath=\"").arg(
                    QDir::toNativeSeparators(path));
        return false;
    }
    startPos += prfxpath.length();
    int endPos = content.indexOf(char(0), startPos);
    if (endPos == -1) {
        *errorMessage = QString::fromLatin1("Unable to patch %1: Internal error").arg(
                    QDir::toNativeSeparators(path));
        return false;
    }

    QByteArray replacement = QByteArray(endPos - startPos, char(0));
    replacement[0] = '.';
    content.replace(startPos, endPos - startPos, replacement);

    if (!file.seek(0)
            || (file.write(content) != content.size())) {
        *errorMessage = QString::fromLatin1("Unable to patch %1: Could not write to file").arg(
                    QDir::toNativeSeparators(path));
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------

bool Deployment::deployTranslations(const QString &sourcePath, quint64 usedQtModules, const QString &target, unsigned flags, QString *errorMessage)
{
    // Find available languages prefixes by checking on qtbase.
    QStringList prefixes;
    QDir sourceDir(sourcePath);
    const QStringList qmFilter = QStringList(QStringLiteral("qtbase_*.qm"));
    foreach (QString qmFile, sourceDir.entryList(qmFilter)) {
        qmFile.chop(3);
        qmFile.remove(0, 7);
        prefixes.push_back(qmFile);
    }
    if (prefixes.isEmpty()) {
        std::wcerr << "Warning: Could not find any translations in "
                   << QDir::toNativeSeparators(sourcePath) << " (developer build?)\n.";
        return true;
    }
    // Run lconvert to concatenate all files into a single named "qt_<prefix>.qm" in the application folder
    // Use QT_INSTALL_TRANSLATIONS as working directory to keep the command line short.
    const QString absoluteTarget = QFileInfo(target).absoluteFilePath();
    const QString binary = QStringLiteral("lconvert");
    QStringList arguments;
    foreach (const QString &prefix, prefixes) {
        const QString targetFile = QStringLiteral("qt_") + prefix + QStringLiteral(".qm");
        arguments.append(QStringLiteral("-o"));
        arguments.append(QDir::toNativeSeparators(absoluteTarget + QLatin1Char('/') + targetFile));
        foreach (const QString &qmFile, sourceDir.entryList(translationNameFilters(usedQtModules, prefix)))
            arguments.append(qmFile);
        if (optVerboseLevel)
            std::wcout << "Creating " << targetFile << "...\n";
        unsigned long exitCode;
        if (!(flags & SkipUpdateFile)
                && (!runProcess(binary, arguments, sourcePath, &exitCode, 0, 0, errorMessage) || exitCode)) {
            return false;
        }
    } // for prefixes.
    return true;
}

QStringList Deployment::compilerRunTimeLibs(Platform platform, bool isDebug, unsigned wordSize)
{
    QStringList result;
    switch (platform) {
    case WindowsMinGW: { // MinGW: Add runtime libraries
        static const char *minGwRuntimes[] = {"*gcc_", "*stdc++", "*winpthread"};
        const QString gcc = findInPath(QStringLiteral("g++.exe"));
        if (gcc.isEmpty()) {
            std::wcerr << "Warning: Cannot find GCC installation directory. g++.exe must be in the path.\n";
            break;
        }
        const QString binPath = QFileInfo(gcc).absolutePath();
        QDir dir(binPath);
        QStringList filters;
        const QString suffix = QLatin1Char('*') + sharedLibrarySuffix(platform);
        const size_t count = sizeof(minGwRuntimes) / sizeof(minGwRuntimes[0]);
        for (size_t i = 0; i < count; ++i)
            filters.append(QLatin1String(minGwRuntimes[i]) + suffix);
        foreach (const QString &dll, dir.entryList(filters, QDir::Files))
            result.append(binPath + QLatin1Char('/') + dll);
    }
        break;
    case Windows: { // MSVC/Desktop: Add redistributable packages.
        const char vcDirVar[] = "VCINSTALLDIR";
        const QChar slash(QLatin1Char('/'));
        QString vcRedistDirName = QDir::cleanPath(QFile::decodeName(qgetenv(vcDirVar)));
        if (vcRedistDirName.isEmpty()) {
            std::wcerr << "Warning: Cannot find Visual Studio installation directory, " << vcDirVar << " is not set.\n";
            break;
        }
        if (!vcRedistDirName.endsWith(slash))
            vcRedistDirName.append(slash);
        vcRedistDirName.append(QStringLiteral("redist"));
        QDir vcRedistDir(vcRedistDirName);
        if (!vcRedistDir.exists()) {
            std::wcerr << "Warning: Cannot find Visual Studio redist directory, "
                       << QDir::toNativeSeparators(vcRedistDirName).toStdWString() << ".\n";
            break;
        }
        QStringList redistFiles;
        const QString wordSizeString(QLatin1String(wordSize > 32 ? "x64" : "x86"));
        if (isDebug) {
            // Append DLLs from Debug_NonRedist\x??\Microsoft.VC<version>.DebugCRT.
            if (vcRedistDir.cd(QLatin1String("Debug_NonRedist")) && vcRedistDir.cd(wordSizeString)) {
                const QStringList names = vcRedistDir.entryList(QStringList(QStringLiteral("Microsoft.VC*.DebugCRT")), QDir::Dirs);
                if (!names.isEmpty() && vcRedistDir.cd(names.first())) {
                    foreach (const QFileInfo &dll, vcRedistDir.entryInfoList(QStringList(QLatin1String("*.dll"))))
                        redistFiles.append(dll.absoluteFilePath());
                }
            }
        } else { // release: Bundle vcredist<>.exe
            const QStringList countryCodes = vcRedistDir.entryList(QStringList(QStringLiteral("[0-9]*")), QDir::Dirs);
            if (!countryCodes.isEmpty()) {
                const QFileInfo fi(vcRedistDirName + slash + countryCodes.first() + slash
                                   + QStringLiteral("vcredist_") + wordSizeString
                                   + QStringLiteral(".exe"));
                if (fi.isFile())
                    redistFiles.append(fi.absoluteFilePath());
            }
        }
        if (redistFiles.isEmpty()) {
            std::wcerr << "Warning: Cannot find Visual Studio " << (isDebug ? "debug" : "release")
                       << " redistributable files in " << QDir::toNativeSeparators(vcRedistDirName).toStdWString() << ".\n";
            break;
        }
        result.append(redistFiles);
    }
    default:
        break;
    }
    return result;
}

Deployment::Deployment(const Options &options, const QMap<QString, QString> &qmakeVariables) :
    m_options(options), m_qmakeVariables(qmakeVariables)
{}

DeployResult Deployment::deploy(const Options &options, QString *errorMessage)
{
    DeployResult result;

    const QChar slash = QLatin1Char('/');

    const QString qtBinDir = m_qmakeVariables.value(QStringLiteral("QT_INSTALL_BINS"));
    const QString libraryLocation = options.platform == Unix ? m_qmakeVariables.value(QStringLiteral("QT_INSTALL_LIBS")) : qtBinDir;
    const int version = qtVersion(m_qmakeVariables);
    Q_UNUSED(version)

    if (optVerboseLevel > 1)
        std::wcout << "Qt binaries in " << QDir::toNativeSeparators(qtBinDir) << '\n';

    QStringList dependentQtLibs;
    bool detectedDebug;
    unsigned wordSize;
    int directDependencyCount = 0;
    if (!findDependentQtLibraries(libraryLocation, options.binaries.first(), options.platform, errorMessage, &dependentQtLibs, &wordSize,
                                  &detectedDebug, &directDependencyCount)) {
        return result;
    }
    for (int b = 1; b < options.binaries.size(); ++b) {
        if (!findDependentQtLibraries(libraryLocation, options.binaries.at(b), options.platform, errorMessage, &dependentQtLibs,
                                      Q_NULLPTR, Q_NULLPTR, Q_NULLPTR)) {
            return result;
        }
    }

    const bool isDebug = options.debugDetection == Options::DebugDetectionAuto ? detectedDebug: options.debugDetection == Options::DebugDetectionForceDebug;
    const DebugMatchMode debugMatchMode = options.debugMatchAll
            ? MatchDebugOrRelease : (isDebug ? MatchDebug : MatchRelease);

    // Determine application type, check Quick2 is used by looking at the
    // direct dependencies (do not be fooled by QtWebKit depending on it).
    QString qtLibInfix;
    for (int m = 0; m < directDependencyCount; ++m) {
        const quint64 module = qtModule(dependentQtLibs.at(m));
        result.directlyUsedQtLibraries |= module;
        if (module == QtCoreModule)
            qtLibInfix = qtlibInfixFromCoreLibName(dependentQtLibs.at(m), detectedDebug, options.platform);
    }

    const bool usesQml2 = !(options.disabledLibraries & QtQmlModule)
            && ((result.directlyUsedQtLibraries & (QtQmlModule | QtQuickModule | Qt3DQuickModule))
                || (options.additionalLibraries & QtQmlModule));

    if (optVerboseLevel) {
        std::wcout << QDir::toNativeSeparators(options.binaries.first()) << ' '
                   << wordSize << " bit, " << (isDebug ? "debug" : "release")
                   << " executable";
        if (usesQml2)
            std::wcout << " [QML]";
        std::wcout << '\n';
    }

    if (dependentQtLibs.isEmpty()) {
        *errorMessage = QDir::toNativeSeparators(options.binaries.first()) +  QStringLiteral(" does not seem to be a Qt executable.");
        return result;
    }

    // Some Windows-specific checks: Qt5Core depends on ICU when configured with "-icu". Other than
    // that, Qt5WebKit has a hard dependency on ICU.
    if (options.platform & WindowsBased)  {
        const QStringList qtLibs = dependentQtLibs.filter(QStringLiteral("Qt5Core"), Qt::CaseInsensitive)
                + dependentQtLibs.filter(QStringLiteral("Qt5WebKit"), Qt::CaseInsensitive);
        foreach (const QString &qtLib, qtLibs) {
            QStringList icuLibs = findDependentLibraries(qtLib, options.platform, errorMessage).filter(QStringLiteral("ICU"), Qt::CaseInsensitive);
            if (!icuLibs.isEmpty()) {
                // Find out the ICU version to add the data library icudtXX.dll, which does not show
                // as a dependency.
                QRegExp numberExpression(QStringLiteral("\\d+"));
                Q_ASSERT(numberExpression.isValid());
                const int index = numberExpression.indexIn(icuLibs.front());
                if (index >= 0)  {
                    const QString icuVersion = icuLibs.front().mid(index, numberExpression.matchedLength());
                    if (optVerboseLevel > 1)
                        std::wcout << "Adding ICU version " << icuVersion << '\n';
                    icuLibs.push_back(QStringLiteral("icudt") + icuVersion + QLatin1String(windowsSharedLibrarySuffix));
                }
                foreach (const QString &icuLib, icuLibs) {
                    const QString icuPath = findInPath(icuLib);
                    if (icuPath.isEmpty()) {
                        *errorMessage = QStringLiteral("Unable to locate ICU library ") + icuLib;
                        return result;
                    }
                    dependentQtLibs.push_back(icuPath);
                } // foreach icuLib
                break;
            } // !icuLibs.isEmpty()
        } // Qt5Core/Qt5WebKit
    } // Windows

    // Scan Quick2 imports
    QmlImportScanResult qmlScanResult;
    if (options.quickImports && usesQml2) {
        QStringList qmlDirectories = options.qmlDirectories;
        if (qmlDirectories.isEmpty()) {
            const QString qmlDirectory = findQmlDirectory(options.platform, options.directory);
            if (!qmlDirectory.isEmpty())
                qmlDirectories.append(qmlDirectory);
        }
        foreach (const QString &qmlDirectory, qmlDirectories) {
            if (optVerboseLevel >= 1)
                std::wcout << "Scanning " << QDir::toNativeSeparators(qmlDirectory) << ":\n";
            const QmlImportScanResult scanResult = runQmlImportScanner(qmlDirectory, m_qmakeVariables.value(QStringLiteral("QT_INSTALL_QML")), options.platform,
                                                                       debugMatchMode, errorMessage);
            if (!scanResult.ok)
                return result;
            qmlScanResult.append(scanResult);
            // Additional dependencies of QML plugins.
            foreach (const QString &plugin, qmlScanResult.plugins) {
                if (!findDependentQtLibraries(libraryLocation, plugin, options.platform, errorMessage, &dependentQtLibs, &wordSize, &detectedDebug))
                    return result;
            }
            if (optVerboseLevel >= 1) {
                std::wcout << "QML imports:\n";
                foreach (const QmlImportScanResult::Module &mod, qmlScanResult.modules) {
                    std::wcout << "  '" << mod.name << "' "
                               << QDir::toNativeSeparators(mod.sourcePath) << '\n';
                }
                if (optVerboseLevel >= 2) {
                    std::wcout << "QML plugins:\n";
                    foreach (const QString &p, qmlScanResult.plugins)
                        std::wcout << "  " << QDir::toNativeSeparators(p) << '\n';
                }
            }
        }
    }

    // Find the plugins and check whether ANGLE, D3D are required on the platform plugin.
    QString platformPlugin;
    // Sort apart Qt 5 libraries in the ones that are represented by the
    // QtModule enumeration (and thus controlled by flags) and others.
    QStringList deployedQtLibraries;
    for (int i = 0 ; i < dependentQtLibs.size(); ++i)  {
        if (const quint64 qtm = qtModule(dependentQtLibs.at(i)))
            result.usedQtLibraries |= qtm;
        else
            deployedQtLibraries.push_back(dependentQtLibs.at(i)); // Not represented by flag.
    }
    result.deployedQtLibraries = (result.usedQtLibraries | options.additionalLibraries) & ~options.disabledLibraries;

    const QStringList plugins =
            findQtPlugins(&result.deployedQtLibraries,
                          // For non-QML applications, disable QML to prevent it from being pulled in by the qtaccessiblequick plugin.
                          options.disabledLibraries | (usesQml2 ? 0 : (QtQmlModule | QtQuickModule)),
                          m_qmakeVariables.value(QStringLiteral("QT_INSTALL_PLUGINS")), libraryLocation,
                          debugMatchMode, options.platform, &platformPlugin);

    // Apply options flags and re-add library names.
    QString qtGuiLibrary;
    for (size_t i = 0; i < qtModuleEntryCount(); ++i) {
        if (result.deployedQtLibraries & qtModuleEntryByIndex(i).module) {
            const QString library = libraryPath(libraryLocation, qtModuleEntryByIndex(i).libraryName, qtLibInfix, options.platform, isDebug);
            deployedQtLibraries.append(library);
            if (qtModuleEntryByIndex(i).module == QtGuiModule)
                qtGuiLibrary = library;
        }
    }

    if (optVerboseLevel >= 1) {
        std::wcout << "Direct dependencies: " << formatQtModules(result.directlyUsedQtLibraries).constData()
                   << "\nAll dependencies   : " << formatQtModules(result.usedQtLibraries).constData()
                   << "\nTo be deployed     : " << formatQtModules(result.deployedQtLibraries).constData() << '\n';
    }

    if (optVerboseLevel > 1)
        std::wcout << "Plugins: " << plugins.join(QLatin1Char(',')) << '\n';

    if ((result.deployedQtLibraries & QtGuiModule) && platformPlugin.isEmpty()) {
        *errorMessage =QStringLiteral("Unable to find the platform plugin.");
        return result;
    }

    // Check for ANGLE on the Qt5Gui library.
    if ((options.platform & WindowsBased) && options.platform != WinCEIntel
            && options.platform != WinCEArm && !qtGuiLibrary.isEmpty())  {
        QString libGlesName = QStringLiteral("libGLESV2");
        if (isDebug)
            libGlesName += QLatin1Char('d');
        libGlesName += QLatin1String(windowsSharedLibrarySuffix);
        const QStringList guiLibraries = findDependentLibraries(qtGuiLibrary, options.platform, errorMessage);
        const bool dependsOnAngle = !guiLibraries.filter(libGlesName, Qt::CaseInsensitive).isEmpty();
        const bool dependsOnOpenGl = !guiLibraries.filter(QStringLiteral("opengl32"), Qt::CaseInsensitive).isEmpty();
        if (options.angleDetection != Options::AngleDetectionForceOff
                && (dependsOnAngle || !dependsOnOpenGl || options.angleDetection == Options::AngleDetectionForceOn)) {
            const QString libGlesFullPath = qtBinDir + slash + libGlesName;
            deployedQtLibraries.append(libGlesFullPath);
            QString libEglFullPath = qtBinDir + slash + QStringLiteral("libEGL");
            if (isDebug)
                libEglFullPath += QLatin1Char('d');
            libEglFullPath += QLatin1String(windowsSharedLibrarySuffix);
            deployedQtLibraries.append(libEglFullPath);
            // Find the system D3d Compiler matching the D3D library.
            if (options.systemD3dCompiler && !options.isWinRtOrWinPhone()) {
                const QString d3dCompiler = findD3dCompiler(options.platform, qtBinDir, wordSize);
                if (d3dCompiler.isEmpty()) {
                    std::wcerr << "Warning: Cannot find any version of the d3dcompiler DLL.\n";
                } else {
                    deployedQtLibraries.push_back(d3dCompiler);
                }
            }
        } // deployAngle
        if (!dependsOnOpenGl) {
            const QFileInfo softwareRasterizer(qtBinDir + slash + QStringLiteral("opengl32sw") + QLatin1String(windowsSharedLibrarySuffix));
            if (softwareRasterizer.isFile())
                deployedQtLibraries.append(softwareRasterizer.absoluteFilePath());
        }
    } // Windows

    // Update libraries
    if (options.libraries) {
        const QString targetPath = options.libraryDirectory.isEmpty() ?
                    options.directory : options.libraryDirectory;
        QStringList libraries = deployedQtLibraries;
        if (options.compilerRunTime)
            libraries.append(compilerRunTimeLibs(options.platform, isDebug, wordSize));
        foreach (const QString &qtLib, libraries) {
            if (!updateFile(qtLib, targetPath, options.updateFileFlags, options.json, errorMessage))
                return result;
        }

        if (!options.isWinRtOrWinPhone()) {
            const QString qt5CoreName = QFileInfo(libraryPath(libraryLocation, "Qt5Core", qtLibInfix,
                                                              options.platform, isDebug)).fileName();

            if (!patchQtCore(targetPath + QLatin1Char('/') + qt5CoreName, errorMessage))
                return result;
        }
    } // optLibraries

    // Update plugins
    if (options.plugins) {
        QDir dir(options.directory);
        foreach (const QString &plugin, plugins) {
            const QString targetDirName = plugin.section(slash, -2, -2);
            if (!dir.exists(targetDirName)) {
                if (optVerboseLevel)
                    std::wcout << "Creating directory " << targetDirName << ".\n";
                if (!(options.updateFileFlags & SkipUpdateFile) && !dir.mkdir(targetDirName)) {
                    std::wcerr << "Cannot create " << targetDirName << ".\n";
                    *errorMessage = QStringLiteral("Cannot create ") + targetDirName +  QLatin1Char('.');
                    return result;
                }
            }
            const QString targetPath = options.directory + slash + targetDirName;
            if (!updateFile(plugin, targetPath, options.updateFileFlags, options.json, errorMessage))
                return result;
        }
    } // optPlugins

    // Update Quick imports
    const bool usesQuick1 = result.deployedQtLibraries & QtDeclarativeModule;
    // Do not be fooled by QtWebKit.dll depending on Quick into always installing Quick imports
    // for WebKit1-applications. Check direct dependency only.
    if (options.quickImports && (usesQuick1 || usesQml2)) {
        const QmlDirectoryFileEntryFunction qmlFileEntryFunction(options.platform, debugMatchMode);
        if (usesQml2) {
            foreach (const QmlImportScanResult::Module &module, qmlScanResult.modules) {
                const QString installPath = module.installPath(options.directory);
                if (optVerboseLevel > 1)
                    std::wcout << "Installing: '" << module.name
                               << "' from " << module.sourcePath << " to "
                               << QDir::toNativeSeparators(installPath) << '\n';
                if (installPath != options.directory && !createDirectory(installPath, errorMessage))
                    return result;
                const bool updateResult = module.sourcePath.contains(QLatin1String("QtQuick/Controls"))
                        || module.sourcePath.contains(QLatin1String("QtQuick/Dialogs")) ?
                            updateFile(module.sourcePath, QmlDirectoryFileEntryFunction(options.platform, debugMatchMode, true),
                                       installPath, options.updateFileFlags | RemoveEmptyQmlDirectories,
                                       options.json, errorMessage) :
                            updateFile(module.sourcePath, qmlFileEntryFunction, installPath, options.updateFileFlags,
                                       options.json, errorMessage);
                if (!updateResult)
                    return result;
            }
        } // Quick 2
        if (usesQuick1) {
            const QString quick1ImportPath = m_qmakeVariables.value(QStringLiteral("QT_INSTALL_IMPORTS"));
            QStringList quick1Imports(QStringLiteral("Qt"));
            if (result.deployedQtLibraries & QtWebKitModule)
                quick1Imports << QStringLiteral("QtWebKit");
            foreach (const QString &quick1Import, quick1Imports) {
                const QString sourceFile = quick1ImportPath + slash + quick1Import;
                if (!updateFile(sourceFile, qmlFileEntryFunction, options.directory, options.updateFileFlags, options.json, errorMessage))
                    return result;
            }
        } // Quick 1
    } // optQuickImports

    if (options.translations) {
        if (!createDirectory(options.translationsDirectory, errorMessage)
                || !deployTranslations(m_qmakeVariables.value(QStringLiteral("QT_INSTALL_TRANSLATIONS")),
                                       result.deployedQtLibraries, options.translationsDirectory,
                                       options.updateFileFlags, errorMessage)) {
            return result;
        }
    }

    result.success = true;
    return result;
}

bool Deployment::deployWebProcess(const char *binaryName, QString *errorMessage)
{
    // Copy the web process and its dependencies
    const QString webProcess = webProcessBinary(binaryName, m_options.platform);
    const QString webProcessSource = m_qmakeVariables.value(QStringLiteral("QT_INSTALL_LIBEXECS")) +
            QLatin1Char('/') + webProcess;
    if (!updateFile(webProcessSource, m_options.directory, m_options.updateFileFlags, m_options.json, errorMessage))
        return false;

    Options options(m_options);
    options.binaries.append(options.directory + QLatin1Char('/') + webProcess);
    options.quickImports = false;
    options.translations = false;
    return deploy(options, errorMessage);
}

bool Deployment::deployWebEngine(QString *errorMessage)
{
    static const char *installDataFiles[] = {"icudtl.dat",
                                             "qtwebengine_resources.pak",
                                             "qtwebengine_resources_100p.pak",
                                             "qtwebengine_resources_200p.pak"};

    std::wcout << "Deploying: " << Options::webEngineProcessC << "...\n";
    if (!deployWebProcess(Options::webEngineProcessC, errorMessage)) {
        std::wcerr << errorMessage << '\n';
        return false;
    }
    const QString installData = m_qmakeVariables.value(QStringLiteral("QT_INSTALL_DATA")) + QLatin1Char('/');
    for (size_t i = 0; i < sizeof(installDataFiles)/sizeof(installDataFiles[0]); ++i) {
        if (!updateFile(installData + QLatin1String(installDataFiles[i]),
                        m_options.directory, m_options.updateFileFlags, m_options.json, errorMessage)) {
            std::wcerr << errorMessage << '\n';
            return false;
        }
    }
    const QFileInfo translations(m_qmakeVariables.value(QStringLiteral("QT_INSTALL_TRANSLATIONS"))
                                 + QStringLiteral("/qtwebengine_locales"));
    if (!translations.isDir()) {
        std::wcerr << "Warning: Cannot find the translation files of the QtWebEngine module at "
                   << QDir::toNativeSeparators(translations.absoluteFilePath()) << '.';
        return true;
    }
    // Missing translations may cause crashes, ignore --no-translations.
    return createDirectory(m_options.translationsDirectory, errorMessage)
            && updateFile(translations.absoluteFilePath(), m_options.translationsDirectory,
                          m_options.updateFileFlags, m_options.json, errorMessage);
}

QT_END_NAMESPACE
