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

#ifndef QTMODULES_H
#define QTMODULES_H

#include "types.h"

QT_BEGIN_NAMESPACE

enum QtModule
#if defined(Q_COMPILER_CLASS_ENUM) || defined(Q_CC_MSVC)
    : quint64
#endif
{
    QtBluetoothModule         = 0x000000000001,
    QtCLuceneModule           = 0x000000000002,
    QtConcurrentModule        = 0x000000000004,
    QtCoreModule              = 0x000000000008,
    QtDeclarativeModule       = 0x000000000010,
    QtDesignerComponents      = 0x000000000020,
    QtDesignerModule          = 0x000000000040,
    QtGuiModule               = 0x000000000080,
    QtCluceneModule           = 0x000000000100,
    QtHelpModule              = 0x000000000200,
    QtMultimediaModule        = 0x000000000400,
    QtMultimediaWidgetsModule = 0x000000000800,
    QtMultimediaQuickModule   = 0x000000001000,
    QtNetworkModule           = 0x000000002000,
    QtNfcModule               = 0x000000004000,
    QtOpenGLModule            = 0x000000008000,
    QtPositioningModule       = 0x000000010000,
    QtPrintSupportModule      = 0x000000020000,
    QtQmlModule               = 0x000000040000,
    QtQuickModule             = 0x000000080000,
    QtQuickParticlesModule    = 0x000000100000,
    QtScriptModule            = 0x000000200000,
    QtScriptToolsModule       = 0x000000400000,
    QtSensorsModule           = 0x000000800000,
    QtSerialPortModule        = 0x000001000000,
    QtSqlModule               = 0x000002000000,
    QtSvgModule               = 0x000004000000,
    QtTestModule              = 0x000008000000,
    QtWidgetsModule           = 0x000010000000,
    QtWinExtrasModule         = 0x000020000000,
    QtXmlModule               = 0x000040000000,
    QtXmlPatternsModule       = 0x000080000000,
    QtWebKitModule            = 0x000100000000,
    QtWebKitWidgetsModule     = 0x000200000000,
    QtQuickWidgetsModule      = 0x000400000000,
    QtWebSocketsModule        = 0x000800000000,
    QtEnginioModule           = 0x001000000000,
    QtWebEngineCoreModule     = 0x002000000000,
    QtWebEngineModule         = 0x004000000000,
    QtWebEngineWidgetsModule  = 0x008000000000,
    QtQmlToolingModule        = 0x010000000000,
    Qt3DCoreModule            = 0x020000000000,
    Qt3DRendererModule        = 0x040000000000,
    Qt3DQuickModule           = 0x080000000000,
    Qt3DQuickRendererModule   = 0x100000000000,
    Qt3DInputModule           = 0x200000000000,
    QtLocationModule          = 0x400000000000,
    QtWebChannelModule        = 0x800000000000
};

struct QtModuleEntry {
    quint64 module;
    const char *option;
    const char *libraryName;
    const char *translation;
};

size_t qtModuleEntryCount();
QtModuleEntry qtModuleEntryByIndex(int idx);

inline QByteArray formatQtModules(quint64 mask, bool option = false)
{
    QByteArray result;
    for (size_t i = 0; i < qtModuleEntryCount(); ++i) {
        if (mask & qtModuleEntryByIndex(i).module) {
            if (!result.isEmpty())
                result.append(' ');
            result.append(option ? qtModuleEntryByIndex(i).option : qtModuleEntryByIndex(i).libraryName);
        }
    }
    return result;
}

inline bool isQtModule(const QString &libName)
{
    // Match Standard modules, Qt5XX.dll, Qt[Commercial]Charts.dll and special cases.
    return libName.size() > 2
        && ((libName.startsWith(QLatin1String("Qt"), Qt::CaseInsensitive) && libName.at(2).isDigit())
            || libName.startsWith(QLatin1String("QtCommercial"), Qt::CaseInsensitive)
            || libName.startsWith(QLatin1String("QtCharts"), Qt::CaseInsensitive)
            || libName.startsWith(QLatin1String("DataVisualization"), Qt::CaseInsensitive)
            || libName.startsWith(QLatin1String("Enginio"), Qt::CaseInsensitive));
}

inline quint64 qtModuleForPlugin(const QString &subDirName)
{
    if (subDirName == QLatin1String("accessible") || subDirName == QLatin1String("iconengines")
        || subDirName == QLatin1String("imageformats") || subDirName == QLatin1String("platforms")
        || subDirName == QLatin1String("platforminputcontexts")) {
        return QtGuiModule;
    }
    if (subDirName == QLatin1String("bearer"))
        return QtNetworkModule;
    if (subDirName == QLatin1String("sqldrivers"))
        return QtSqlModule;
    if (subDirName == QLatin1String("audio") || subDirName == QLatin1String("mediaservice") || subDirName == QLatin1String("playlistformats"))
        return QtMultimediaModule;
    if (subDirName == QLatin1String("printsupport"))
        return QtPrintSupportModule;
    if (subDirName == QLatin1String("scenegraph"))
        return QtQuickModule;
    if (subDirName == QLatin1String("qmltooling"))
        return QtQuickModule | QtQmlToolingModule;
    if (subDirName == QLatin1String("qml1tooling"))
        return QtDeclarativeModule;
    if (subDirName == QLatin1String("position"))
        return QtPositioningModule;
    if (subDirName == QLatin1String("geoservices"))
        return QtLocationModule;
    if (subDirName == QLatin1String("sensors") || subDirName == QLatin1String("sensorgestures"))
        return QtSensorsModule;
    if (subDirName == QLatin1String("qtwebengine"))
        return QtWebEngineModule | QtWebEngineCoreModule | QtWebEngineWidgetsModule;
    if (subDirName == QLatin1String("sceneparsers"))
        return Qt3DRendererModule;
    return 0; // "designer"
}

inline quint64 qtModule(const QString &module)
{
    quint64 bestMatch = 0;
    int bestMatchLength = 0;
    for (size_t i = 0; i < qtModuleEntryCount(); ++i) {
        const QString libraryName = QLatin1String(qtModuleEntryByIndex(i).libraryName);
        if (libraryName.size() > bestMatchLength && module.contains(libraryName, Qt::CaseInsensitive)) {
            bestMatch = qtModuleEntryByIndex(i).module;
            bestMatchLength = libraryName.size();
        }
    }
    return bestMatch;
}

inline QStringList translationNameFilters(quint64 modules, const QString &prefix)
{
    QStringList result;
    for (size_t i = 0; i < qtModuleEntryCount(); ++i) {
        if ((qtModuleEntryByIndex(i).module & modules) && qtModuleEntryByIndex(i).translation) {
            const QString name = QLatin1String(qtModuleEntryByIndex(i).translation) +
                                 QLatin1Char('_') +  prefix + QStringLiteral(".qm");
            if (!result.contains(name))
                result.push_back(name);
        }
    }
    return result;
}

QT_END_NAMESPACE

#endif // QTMODULES_H
