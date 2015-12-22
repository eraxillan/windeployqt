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

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

bool updateFile(const QString &sourceFileName, const QString &targetDirectory, unsigned flags, JsonOutput *json, QString *errorMessage)
{
    return updateFile(sourceFileName, NameFilterFileEntryFunction(QStringList()), targetDirectory, flags, json, errorMessage);
}

DllDirectoryFileEntryFunction::DllDirectoryFileEntryFunction(Platform platform, DebugMatchMode debugMatchMode, const QString &prefix) :
    m_platform(platform), m_debugMatchMode(debugMatchMode), m_prefix(prefix) {}

QStringList DllDirectoryFileEntryFunction::operator()(const QDir &dir) const
{
    return findSharedLibraries(dir, m_platform, m_debugMatchMode, m_prefix);
}

QmlDirectoryFileEntryFunction::QmlDirectoryFileEntryFunction(Platform platform, DebugMatchMode debugMatchMode, bool skipQmlSources)
    : m_qmlNameFilter(QmlDirectoryFileEntryFunction::qmlNameFilters(skipQmlSources))
    , m_dllFilter(platform, debugMatchMode)
{}

QStringList QmlDirectoryFileEntryFunction::operator()(const QDir &dir) const { return m_dllFilter(dir) + m_qmlNameFilter(dir);  }

QStringList QmlDirectoryFileEntryFunction::qmlNameFilters(bool skipQmlSources)
{
    QStringList result;
    result << QStringLiteral("qmldir") << QStringLiteral("*.qmltypes");
    if (!skipQmlSources)
        result << QStringLiteral("*.js") <<  QStringLiteral("*.qml") << QStringLiteral("*.png");
    return result;
}

NameFilterFileEntryFunction::NameFilterFileEntryFunction(const QStringList &nameFilters) : m_nameFilters(nameFilters) {}

QStringList NameFilterFileEntryFunction::operator()(const QDir &dir) const { return dir.entryList(m_nameFilters, QDir::Files); }
