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

#ifndef DEPLOYMENT_H
#define DEPLOYMENT_H

#include "types.h"

class JsonOutput;
struct Options;

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
bool updateFile(const QString &sourceFileName, const QString &targetDirectory, unsigned flags, JsonOutput *json, QString *errorMessage);

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

struct DeployResult
{
    DeployResult() : success(false), directlyUsedQtLibraries(0), usedQtLibraries(0), deployedQtLibraries(0) {}
    operator bool() const { return success; }

    bool success;
    quint64 directlyUsedQtLibraries;
    quint64 usedQtLibraries;
    quint64 deployedQtLibraries;
};

QStringList findQtPlugins(quint64 *usedQtModules, quint64 disabledQtModules,
                          const QString &qtPluginsDirName, const QString &libraryLocation,
                          DebugMatchMode debugMatchModeIn, Platform platform, QString *platformPlugin);

bool deployTranslations(const QString &sourcePath, quint64 usedQtModules,
                        const QString &target, unsigned flags, QString *errorMessage);

QStringList compilerRunTimeLibs(Platform platform, bool isDebug, unsigned wordSize);

DeployResult deploy(const Options &options,
                    const QMap<QString, QString> &qmakeVariables,
                    QString *errorMessage);


bool deployWebProcess(const QMap<QString, QString> &qmakeVariables,
                      const char *binaryName,
                      const Options &sourceOptions, QString *errorMessage);

bool deployWebEngine(const QMap<QString, QString> &qmakeVariables,
                     const Options &options, QString *errorMessage);

QT_END_NAMESPACE

#endif // DEPLOYMENT_H
