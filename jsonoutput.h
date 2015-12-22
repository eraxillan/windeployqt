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

#ifndef JSONOUTPUT_H
#define JSONOUTPUT_H

#include "types.h"

// Container class for JSON output
class JsonOutput
{
    typedef QPair<QString, QString> SourceTargetMapping;
    typedef QList<SourceTargetMapping> SourceTargetMappings;

public:
    void addFile(const QString &source, const QString &target)
    {
        m_files.append(SourceTargetMapping(source, target));
    }

    void removeTargetDirectory(const QString &targetDirectory)
    {
        for (int i = m_files.size() - 1; i >= 0; --i) {
            if (m_files.at(i).second == targetDirectory)
                m_files.removeAt(i);
        }
    }

    QByteArray toJson() const
    {
        QJsonObject document;
        QJsonArray files;
        foreach (const SourceTargetMapping &mapping, m_files) {
            QJsonObject object;
            object.insert(QStringLiteral("source"), QDir::toNativeSeparators(mapping.first));
            object.insert(QStringLiteral("target"), QDir::toNativeSeparators(mapping.second));
            files.append(object);
        }
        document.insert(QStringLiteral("files"), files);
        return QJsonDocument(document).toJson();
    }
    QByteArray toList(ListOption option, const QDir &base) const
    {
        QByteArray list;
        foreach (const SourceTargetMapping &mapping, m_files) {
            const QString source = QDir::toNativeSeparators(mapping.first);
            const QString fileName = QFileInfo(mapping.first).fileName();
            const QString target = QDir::toNativeSeparators(mapping.second) + QDir::separator() + fileName;
            switch (option) {
            case ListNone:
                break;
            case ListSource:
                list += source.toUtf8() + '\n';
                break;
            case ListTarget:
                list += target.toUtf8() + '\n';
                break;
            case ListRelative:
                list += QDir::toNativeSeparators(base.relativeFilePath(target)).toUtf8() + '\n';
                break;
            case ListMapping:
                list += '"' + source.toUtf8() + "\" \"" + QDir::toNativeSeparators(base.relativeFilePath(target)).toUtf8() + "\"\n";
                break;
            }
        }
        return list;
    }
private:
    SourceTargetMappings m_files;
};

#endif // JSONOUTPUT_H
