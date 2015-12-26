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

QByteArray formatQtModules(quint64 mask, bool option = false);
bool isQtModule(const QString &libName);
quint64 qtModuleForPlugin(const QString &subDirName);
quint64 qtModule(const QString &module);

QStringList translationNameFilters(quint64 modules, const QString &prefix);

QT_END_NAMESPACE

#endif // QTMODULES_H
