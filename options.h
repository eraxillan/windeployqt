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

#ifndef OPTIONS_H
#define OPTIONS_H

#include "types.h"

QT_BEGIN_NAMESPACE

class JsonOutput;

struct Options {
    enum DebugDetection {
        DebugDetectionAuto,
        DebugDetectionForceDebug,
        DebugDetectionForceRelease
    };

    enum AngleDetection {
        AngleDetectionAuto,
        AngleDetectionForceOn,
        AngleDetectionForceOff
    };

    Options() : plugins(true), libraries(true), quickImports(true), translations(true), systemD3dCompiler(true), compilerRunTime(false)
              , angleDetection(AngleDetectionAuto), platform(Windows), additionalLibraries(0), disabledLibraries(0)
              , updateFileFlags(0), json(0), list(ListNone), debugDetection(DebugDetectionAuto)
              , debugMatchAll(false) {}

    bool plugins;
    bool libraries;
    bool quickImports;
    bool translations;
    bool systemD3dCompiler;
    bool compilerRunTime;
    AngleDetection angleDetection;
    Platform platform;
    quint64 additionalLibraries;
    quint64 disabledLibraries;
    quint64 updateFileFlags;
    QStringList qmlDirectories; // Project's QML files.
    QString directory;
    QString translationsDirectory; // Translations target directory
    QString libraryDirectory;
    QStringList binaries;
    JsonOutput *json;
    ListOption list;
    DebugDetection debugDetection;
    bool debugMatchAll;

    static const char webKitProcessC[];
    static const char webEngineProcessC[];

    inline bool isWinRtOrWinPhone() const {
        return (platform == WinPhoneArm || platform == WinPhoneIntel
                || platform == WinRtArm || platform == WinRtIntel);
    }
};

QT_END_NAMESPACE

#endif // OPTIONS_H
